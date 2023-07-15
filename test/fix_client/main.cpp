#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

#include "order.h"

void in_queue_consume( fix::fix_client &cli ) {
	auto &cfg = cli.cfg( );
	auto &ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	market::orderbook_t book;

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->in_queue.pop( );
		if ( !msg.buf ) {
			fix_ctx->log->critical( "Message from fd: {} contains invalid buffer.", msg.fd );
			continue;
		}

		const std::string_view buf{ msg.buf, msg.len };
		auto &session = fix_ctx->active_sessions[ msg.fd ];
		if ( !session ) {
			fix_ctx->log->critical( "Received message from inactive FIX session, fd: {}", msg.fd );
			continue;
		}

		const int state = session->state( );
		const auto target_id = session->target_id( );

		fix_ctx->log->debug( "IN:{} -> {}", target_id, buf );

		fix::fix_message_t rd{ buf };

		if ( !rd.validate( ) ) {
			fix_ctx->log->error( "Invalid FIX message received on {} session", target_id );
			ctx->bufpool.release( msg.buf );
			continue;
		}

		char msg_type = rd.msg_type;

		fix_ctx->log->info( "Got message type {} on {} session", msg_type, target_id );

		if ( state == fix::LoggedIn ) {
			if ( msg_type == '1' || msg_type == '0' ) { // TestRequest
				fix_ctx->log->info( "Got heartbeat on {} session", target_id );

				std::memset( msg.buf, 0, ctx->bufpool.obj_size );
				fix::fix_writer wr{ msg.buf, ctx->bufpool.obj_size };

				wr.push_header( session->begin_str( ) );
				wr.push_field( fix_spec::MsgType, '0' );

				wr.push_field( fix_spec::TargetCompID, target_id );
				wr.push_field( fix_spec::SenderCompID, session->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, session->next_seq( ) );
				wr.push_trailer( );

				ctx->out_queue.push( message::net_msg_t{ msg.fd, msg.buf, wr.size( ) } );
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

				fix_ctx->log->info( "{}: BUY: {}, Volume: {}, SELL: {}, Volume: {}", time, buy, buy_vol, sell,
									sell_vol );

				ctx->bufpool.release( msg.buf );
				
				continue;
			}

			fix_ctx->log->warn( "Received unhandled message type: {}, on session: {}", msg_type, target_id );
			ctx->bufpool.release( msg.buf );

		} else if ( state == fix::AwaitingLogon ) { // expect only login confirmation
			if ( msg_type == 'A' ) {
				fix_ctx->log->info( "Got logon confirmation on session {}", target_id );

				session->set_state( fix::LoggedIn );

				fix_ctx->log->info( "Sending subscriptions..." );

				std::memset( msg.buf, 0, ctx->bufpool.obj_size );
				fix::fix_writer wr{ msg.buf, ctx->bufpool.obj_size };

				wr.push_header( session->begin_str( ) );
				wr.push_field( fix_spec::MsgType, 'V' );

				wr.push_field( fix_spec::TargetCompID, target_id );
				wr.push_field( fix_spec::SenderCompID, session->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, session->next_seq( ) );

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

				ctx->out_queue.push( message::net_msg_t{ msg.fd, msg.buf, wr.size( ) } );
				continue;
			}

			fix_ctx->log->warn( "Session {} awaiting logon did not get confirmation.", session->target_id( ) );
			fix_ctx->log->debug( "Session state {}, Message type: {}, Buffer: {}", state, msg_type, buf );
			ctx->bufpool.release( msg.buf );
		}
	}
}

void out_queue_consume( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->out_queue.pop( );

		auto &session = fix_ctx->active_sessions[ msg.fd ];
		std::string_view buf{ msg.buf, msg.len };

		fix_ctx->log->info( "Sending {} to {} session", buf, session->target_id( ) );

		fix_ctx->log->debug( "OUT:{} -> {}", session->target_id( ), buf );

		int nwrite = session->write( buf );
		if ( nwrite <= 0 ) {
			session->reset( );

			fix_ctx->log->error( "Send to FIX session {} returned {}, resetting...", session->target_id( ), nwrite );

			fix_ctx->active_sessions[ msg.fd ].reset( );
		}

		ctx->bufpool.release( msg.buf );
	}
}

int main( ) {
	config::cli_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "failed to parse client config file" );
		return 0;
	}

	fix::fix_client cli( cfg );
	auto &ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	int ret = cli.connect( );
	if ( !ret ) {
		return 0;
	}

	std::thread( out_queue_consume, std::ref( cli ) ).detach( );
	std::thread( in_queue_consume, std::ref( cli ) ).detach( );

	for ( auto &[ fd, session ] : fix_ctx->active_sessions ) {
		fix_ctx->log->info( "Sending Logon to {}...", session->target_id( ) );

		auto buf = ctx->bufpool.get( );

		fix::fix_writer wr{ buf, ctx->bufpool.obj_size };

		wr.push_header( session->begin_str( ) );
		wr.push_field( fix_spec::MsgType, 'A' );

		wr.push_field( fix_spec::TargetCompID, session->target_id( ) );
		wr.push_field( fix_spec::SenderCompID, session->sender_id( ) );
		wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
		wr.push_field( fix_spec::MsgSeqNum, session->next_seq( ) );
		wr.push_field( fix_spec::ResetSeqNumFlag, 'Y' );
		wr.push_field( fix_spec::EncryptMethod, 0 );
		wr.push_field( fix_spec::HeartBtInt, 30 );

		wr.push_field( fix_spec::Username, session->sender_id( ) );
		wr.push_field( fix_spec::Password, session->sender_id( ) );
		wr.push_trailer( );

		ctx->out_queue.push( message::net_msg_t{ fd, buf, wr.size( ) } );
	}

	cli.run( );

	return 0;
}