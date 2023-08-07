#include <include.h>

#include <tcp_client.h>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"

std::atomic_int g_cur_latency = 0;

struct read_t;
using met_cli = net::tcp_client_impl< 64, 64, 128, 64, net::tcp_msg_t, true, read_t >;

std::unique_ptr< met_cli > g_cli;

struct read_t {
	bool operator( )( const net::tcp_msg_t &msg ) {
		std::string_view buf{ msg.buf, msg.len };
		const auto latency = g_cli->clock( )->rdns( ) - g_cli->clock( )->tsc2ns( msg.timestamp );
		spdlog::info( "{}, latency: {}ns", buf, latency );

		g_cli->post( std::move( msg ) );
		return false;
	}
};

void tcp_metrics_server( ) {
	prometheus::Exposer exp{ "localhost:9015" };

	auto registry = std::make_shared< prometheus::Registry >( );
	auto &gauge_family = prometheus::BuildGauge( ).Name( "tcp_states" ).Register( *registry );

	auto &push_time = gauge_family.Add( { { "time", "queue_latency" } } );

	exp.RegisterCollectable( registry );

	for ( ;; ) {
		push_time.Set( g_cur_latency );

		std::this_thread::sleep_for( std::chrono::seconds{ 1 } ); // update the counters every 1s
	}
}

int main( ) {
	g_cli = std::make_unique< met_cli >( "TCP" );

	if ( g_cli->connect( "127.0.0.1", "1515" ) != 0 ) {
		spdlog::error( "Failed to connect to metrics server." );
		return 0;
	}

	// std::thread( tcp_metrics_server ).detach( );

	g_cli->poll( );

	return 0;
}