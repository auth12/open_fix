#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

#include "order.h"

#include "ipc.h"

void in_queue_consume( std::unique_ptr< fix::fix_client > &cli ) {
	auto log = cli->m_log;

	while ( cli->fd( ) > 0 ) {
		net::tcp_msg_t msg{ };
		if ( !cli->consume_in( msg ) )
			continue;

		const std::string_view buf{ msg.buf.data( ), msg.buf.size( ) };

		const int state = cli->state( );
		const auto target_id = cli->target_id( );

		log->debug( "IN:{} -> {}", target_id, buf );

		fix::fix_message_t rd{ buf };

		if ( !rd.validate( ) ) {
			log->error( "Invalid FIX message received on {} session", target_id );
			cli->release_buf( msg.buf.data( ) );
			continue;
		}

		char msg_type = rd.msg_type;

		log->info( "Got message type {} on {} session", msg_type, target_id );

		if ( state == fix::LoggedIn ) {
			if ( msg_type == '1' || msg_type == '0' ) { // TestRequest
				log->info( "Got heartbeat on {} session", target_id );

				std::memset( msg.buf.data( ), 0, net::buffer_size );
				fix::fix_writer wr{ msg.buf.data( ), net::buffer_size };

				wr.push_header( cli->begin_str( ) );
				wr.push_field( fix_spec::MsgType, '0' );

				wr.push_field( fix_spec::TargetCompID, target_id );
				wr.push_field( fix_spec::SenderCompID, cli->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, cli->next_seq( ) );
				wr.push_trailer( );

				cli->post( msg.buf.data( ), wr.size( ) );

				continue;
			}

			if ( msg_type == 'W' ) {
				double buy, sell = 0;
				double buy_vol, sell_vol = 0;
				std::string_view time;

				char side = 0;
				for ( auto it = rd.begin( ); it < rd.end( ); ++it ) {
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

				log->info( "{}: BUY: {}, Volume: {}, SELL: {}, Volume: {}", time, buy, buy_vol, sell, sell_vol );

				cli->release_buf( msg.buf.data( ) );

				continue;
			}

			log->warn( "Received unhandled message type: {}, on session: {}", msg_type, target_id );
			cli->release_buf( msg.buf.data( ) );

		} else if ( state == fix::AwaitingLogon ) { // expect only login confirmation
			if ( msg_type == 'A' ) {
				log->info( "Got logon confirmation on session {}", target_id );

				cli->set_state( fix::LoggedIn );

				log->info( "Sending subscriptions..." );

				std::memset( msg.buf.data( ), 0, net::buffer_size );
				fix::fix_writer wr{ msg.buf.data( ), net::buffer_size };

				wr.push_header( cli->begin_str( ) );
				wr.push_field( fix_spec::MsgType, 'V' );

				wr.push_field( fix_spec::TargetCompID, target_id );
				wr.push_field( fix_spec::SenderCompID, cli->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, cli->next_seq( ) );

				wr.push_field( fix_spec::MDReqID, "eth" );
				wr.push_field( fix_spec::SubscriptionRequestType, '1' );
				wr.push_field( fix_spec::MarketDepth, 0 );

				wr.push_field( fix_spec::NoMDEntryTypes, 2 );
				wr.push_field( fix_spec::MDEntryType, '0' );
				wr.push_field( fix_spec::MDEntryType, '1' );

				wr.push_field( fix_spec::NoRelatedSym, 1 );
				wr.push_field( fix_spec::SecurityID, "5002" );
				wr.push_field( fix_spec::SecurityIDSource, '8' );

				wr.push_trailer( );

				cli->post( msg.buf.data( ), wr.size( ) );
				continue;
			}

			log->warn( "Session {} awaiting logon did not get confirmation.", target_id );
			log->debug( "Session state {}, Message type: {}, Buffer: {}", state, msg_type, buf );
			cli->release_buf( msg.buf.data( ) );
		}
	}

	log->info( "In queue thread done" );
}

int main( ) {
	config::client_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "Failed to parse client config file" );
		return 0;
	}

	auto cli = std::make_unique< fix::fix_client >( cfg.log_name, cfg.to_file );

	cli->set_sender_id( cfg.sender_id );
	cli->set_target_id( cfg.target_id );
	cli->set_begin_str( cfg.fix_ver );

	auto log = cli->m_log;

	const auto host = cfg.host, port = fmt::to_string( cfg.port );

	if ( cli->connect( host, port ) != 0 ) {
		log->error( "Failed to connect to {}:{}", host, port );

		return 0;
	}

	std::thread( in_queue_consume, std::ref( cli ) ).detach( );

	cli->set_state( fix::AwaitingLogon );

	log->info( "Sending Logon to {}...", cli->target_id( ) );

	auto buf = cli->get_buf( );

	fix::fix_writer wr{ buf, net::buffer_size };

	wr.push_header( cli->begin_str( ) );
	wr.push_field( fix_spec::MsgType, 'A' );

	wr.push_field( fix_spec::TargetCompID, cli->target_id( ) );
	wr.push_field( fix_spec::SenderCompID, cli->sender_id( ) );
	wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
	wr.push_field( fix_spec::MsgSeqNum, cli->next_seq( ) );
	wr.push_field( fix_spec::ResetSeqNumFlag, 'Y' );
	wr.push_field( fix_spec::EncryptMethod, 0 );
	wr.push_field( fix_spec::HeartBtInt, 30 );

	wr.push_field( fix_spec::Username, cli->sender_id( ) );
	wr.push_field( fix_spec::Password, cli->sender_id( ) );
	wr.push_trailer( );

	cli->post( buf, wr.size( ) );

	cli->poll( );

	return 0;
}