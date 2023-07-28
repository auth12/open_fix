#include <include.h>

#include <tcp_client.h>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"

std::atomic_int queue_ns = 0;

bool on_read_cb( std::unique_ptr< net::tcp_client_serial > &cli, net::tcp_msg_t msg ) {
	std::string_view buf{ msg.buf.data( ), msg.buf.size( ) };

	queue_ns = details::atoi( buf.data( ), buf.data( ) + buf.size( ) );

	spdlog::info( "Got metrics update, time {}", queue_ns );

	return true;
}

void tcp_metrics_server( ) {
	prometheus::Exposer exp{ "localhost:9015" };

	auto registry = std::make_shared< prometheus::Registry >( );
	auto &gauge_family = prometheus::BuildGauge( ).Name( "tcp_states" ).Register( *registry );

	auto &push_time = gauge_family.Add( { { "time", "queue_latency" } } );

	exp.RegisterCollectable( registry );

	for ( ;; ) {
		push_time.Set( queue_ns );

		std::this_thread::sleep_for( std::chrono::seconds{ 1 } ); // update the counters every 1s
	}
}

void send_thread( std::unique_ptr< net::tcp_client_serial > &cli ) {
	auto buf = cli->get_buf( );
	if ( !buf ) {
		spdlog::error( "failed to get send buf." );
		return;
	}

	const std::string_view cmd{ "/metrics" };
	memcpy( buf, cmd.data( ), cmd.size( ) );
	while ( cli->fd( ) > 0 ) {
		cli->post( buf, cmd.size( ) );
		std::this_thread::sleep_for( std::chrono::seconds{ 1 } );
	}
}

int main( ) {
	auto cli = std::make_unique< net::tcp_client_serial >( "TCP" );

	if ( cli->connect( "127.0.0.1", "1515" ) != 0 ) {
		spdlog::error( "Failed to connect to metrics server." );
		return 0;
	}

	auto fn = std::bind( on_read_cb, std::ref( cli ), std::placeholders::_1 );
	cli->set_read_cb( std::move( fn ) );

	std::thread( tcp_metrics_server ).detach( );
	std::thread( send_thread, std::ref( cli ) ).detach( );

	cli->poll( );

	return 0;
}