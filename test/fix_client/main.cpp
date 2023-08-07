#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

#include "order.h"

#include "ipc.h"

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"

std::unique_ptr< fix::fix_client > g_fix_cli;
std::atomic_int g_msg_count = 0, g_dropped_msg_count = 0, g_cur_latency = 0;

void in_queue_consume( ) {
	while ( g_fix_cli->fd( ) > 0 ) {
		net::tcp_msg_t msg{ };
		// g_fix_cli->clock( )->calibrate( );

		if ( !g_fix_cli->consume_in( msg ) )
			continue;

		g_cur_latency = g_fix_cli->clock( )->rdns( ) - g_fix_cli->clock( )->tsc2ns( msg.timestamp );

		++g_msg_count;

		const std::string_view buf{ msg.buf, msg.len };

		const int state = g_fix_cli->state( );
		const auto target_id = g_fix_cli->target_id( );

		SPDLOG_LOGGER_DEBUG( g_fix_cli->m_log, "IN:{} -> size: {}, msg latency: {}ns", target_id, buf.size( ),
							 g_cur_latency );

		fix::fix_message_t rd{ buf };
		rd.init( );
		if ( !rd.valid ) {
			SPDLOG_LOGGER_ERROR( g_fix_cli->m_log, "Invalid FIX message received on {} session", target_id );
			g_fix_cli->release_buf( msg.buf );
			++g_dropped_msg_count;
			continue;
		}

		char msg_type = rd.type;

		SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "IN:{} -> Message Type: {}, Checksum: {}, Loss: {}\n\t{}", target_id,
							msg_type, rd.checksum, rd.loss, buf );

		if ( rd.loss > 0 ) {
			SPDLOG_LOGGER_WARN( g_fix_cli->m_log, "Malformed single FIX message, lost {} bytes", rd.loss );
		}

		if ( state == fix::LoggedIn ) {
			if ( msg_type == '1' || msg_type == '0' ) { // TestRequest
				SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "Got heartbeat on {} session", target_id );

				g_fix_cli->post_heartbeat( msg.buf );
				continue;
			}

			if ( msg_type == 'W' ) {
				double buy, sell = 0;
				double buy_vol, sell_vol = 0;
				std::string_view time;

				char side = 0;
				for ( auto it = rd.begin( ); it != rd.end( ); ++it ) {
					switch ( it->tag ) {
						case fix_spec::MDEntryType: {
							side = it->val.as_char( );

						} break;
						case fix_spec::MDEntryPx: {
							if ( side == '0' ) {
								buy = it->val.as_double( );
							}

							if ( side == '1' ) {
								sell = it->val.as_double( );
							}
						} break;
						case fix_spec::MDEntrySize: {
							if ( side == '0' ) {
								buy_vol = it->val.as_double( );
							}

							if ( side == '1' ) {
								sell_vol = it->val.as_double( );
							}
						} break;
						case fix_spec::MDEntryTime: {
							time = it->val.as_string( );

						} break;
						default:
							break;
					}
				}

				SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "{}: BUY: {}, Volume: {}, SELL: {}, Volume: {}", time, buy,
									buy_vol, sell, sell_vol );

				g_fix_cli->release_buf( msg.buf );

				continue;
			}

			SPDLOG_LOGGER_WARN( g_fix_cli->m_log, "Received unhandled message type: {}, on session: {}", msg_type,
								target_id );
			g_fix_cli->release_buf( msg.buf );

		} else if ( state == fix::AwaitingLogon ) { // expect only login confirmation
			if ( msg_type == 'A' ) {
				SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "Got logon confirmation on session {}", target_id );

				g_fix_cli->set_state( fix::LoggedIn );

				SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "Sending subscriptions..." );
				g_fix_cli->post_market_sub( msg.buf, { { 5002, 8 } }, "test" );
				continue;
			}

			SPDLOG_LOGGER_WARN( g_fix_cli->m_log, "Session {} awaiting logon did not get confirmation.", target_id );
			SPDLOG_LOGGER_DEBUG( g_fix_cli->m_log, "Session state {}, Message type: {}, Buffer: {}", state, msg_type,
								 buf );
			g_fix_cli->release_buf( msg.buf );
		}
	}

	SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "In queue thread done" );
}

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
	config::client_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "Failed to parse client config file" );
		return 0;
	}

	g_fix_cli = std::make_unique< fix::fix_client >( cfg.log_name, cfg.to_file );

	g_fix_cli->set_sender_id( cfg.sender_id );
	g_fix_cli->set_target_id( cfg.target_id );
	g_fix_cli->set_begin_str( cfg.fix_ver );

	auto log = g_fix_cli->m_log;

	const auto host = cfg.host, port = fmt::to_string( cfg.port );

	if ( g_fix_cli->connect( host, port ) != 0 ) {
		SPDLOG_LOGGER_ERROR( g_fix_cli->m_log, "Failed to connect to {}:{}", host, port );

		return 0;
	}

	std::thread( in_queue_consume ).detach( );
	std::thread( tcp_metrics_server ).detach( );

	g_fix_cli->set_state( fix::AwaitingLogon );

	SPDLOG_LOGGER_INFO( g_fix_cli->m_log, "Sending Logon to {}...", g_fix_cli->target_id( ) );

	auto buf = g_fix_cli->get_buf( );

	fix::fix_writer wr{ buf, net::buffer_size };

	wr.push_header( g_fix_cli->begin_str( ) );
	wr.push_field( fix_spec::MsgType, 'A' );

	wr.push_field( fix_spec::TargetCompID, g_fix_cli->target_id( ) );
	wr.push_field( fix_spec::SenderCompID, g_fix_cli->sender_id( ) );
	wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
	wr.push_field( fix_spec::MsgSeqNum, g_fix_cli->next_seq( ) );
	wr.push_field( fix_spec::ResetSeqNumFlag, 'Y' );
	wr.push_field( fix_spec::EncryptMethod, 0 );
	wr.push_field( fix_spec::HeartBtInt, 60 );

	wr.push_field( fix_spec::Username, g_fix_cli->sender_id( ) );
	wr.push_field( fix_spec::Password, g_fix_cli->sender_id( ) );
	wr.push_trailer( );

	g_fix_cli->post( net::tcp_msg_t{ buf, wr.size( ) } );

	g_fix_cli->poll( );

	return 0;
}