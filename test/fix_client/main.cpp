#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

void in_queue_consume( fix::fix_client &cli ) {
	auto &cfg = cli.cfg( );
	auto ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->in_queue.pop( );
		if ( !msg.buf ) {
			fix_ctx->log->critical( "Message from fd: {} contains invalid buffer.", msg.fd );
			continue;
		}

		const std::string_view buf{ msg.buf, msg.len };
		auto session = fix_ctx->active_sessions[ msg.fd ];

		if ( !session ) {
			fix_ctx->log->critical( "Received message from inactive FIX session, fd: {}", msg.fd );
			continue;
		}

		fix_ctx->log->debug( "IN:{} -> {}", session->target_id( ), buf );

		fix::fix_message_t rd{ buf };

		if ( !rd.validate( ) ) {
			fix_ctx->log->error( "Invalid FIX message received on session {}", session->target_id( ) );
			ctx->bufpool.release( msg.buf );
			continue;
		}

		char msg_type = rd.msg_type;
		const int state = session->state( );

		if ( state == fix::LoggedIn ) {
			if ( msg_type == '1' ) { // TestRequest
				std::memset( msg.buf, 0, ctx->bufpool.obj_size );
				fix::fix_writer wr{ msg.buf, ctx->bufpool.obj_size };

				wr.push_header( session->begin_str( ).c_str( ) );
				wr.push_field( fix_spec::MsgType, '0' );

				wr.push_field( fix_spec::TargetCompID, session->target_id( ) );
				wr.push_field( fix_spec::SenderCompID, session->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, session->next_seq( ) );
				wr.push_trailer( );

				ctx->out_queue.push( message::net_msg_t{ msg.fd, msg.buf, wr.size( ) } );
				continue;
			}

			if ( msg_type == 'W' ) {
				for ( auto &f : rd ) {
					if ( f.tag == fix_spec::MDEntryPx ) {
						fix_ctx->log->info( "got px update: {}", f.val.as_double( ) );
					}
				}
				continue;
			}

			fix_ctx->log->warn( "Received unhandled message type: {}, on session: {}", msg_type,
								session->target_id( ) );
			ctx->bufpool.release( msg.buf );
		} else if ( state == fix::AwaitingLogon ) { // expect only login confirmation
			if ( msg_type == 'A' ) {
				fix_ctx->log->info( "Got logon confirmation on session {}", session->target_id( ) );

				session->set_state( fix::LoggedIn );

				fix_ctx->log->info( "Sending subscriptions..." );

				std::memset( msg.buf, 0, ctx->bufpool.obj_size );
				fix::fix_writer wr{ msg.buf, ctx->bufpool.obj_size };

				wr.push_header( session->begin_str( ).c_str( ) );
				wr.push_field( fix_spec::MsgType, 'V' );

				wr.push_field( fix_spec::TargetCompID, session->target_id( ) );
				wr.push_field( fix_spec::SenderCompID, session->sender_id( ) );

				wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_field( fix_spec::MsgSeqNum, session->next_seq( ) );

				wr.push_field( fix_spec::MDReqID, "test_req_1" );
				wr.push_field( fix_spec::SubscriptionRequestType, '1' );
				wr.push_field( fix_spec::MarketDepth, 1 );

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
	auto ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->out_queue.pop( );

		auto session = std::find( ctx->targets.begin( ), ctx->targets.end( ), msg.fd );
		if ( session == ctx->targets.end( ) ) {
			fix_ctx->log->critical( "Outgoing message to inexistent TCP session, resetting FIX session..." );
			fix_ctx->active_sessions[ msg.fd ].reset( );

			ctx->bufpool.release( msg.buf );
			continue;
		}

		auto fix_session = fix_ctx->active_sessions[ msg.fd ];
		std::string_view buf{ msg.buf, msg.len };

		fix_ctx->log->debug( "OUT:{} -> {}", fix_session->target_id( ), buf );

		ssize_t nwrite = session->write( buf );
		if ( nwrite <= 0 ) {
			session->reset( );
			session->poll_stop( );

			fix_ctx->log->error( "Send to FIX session {} returned {}, resetting...", fix_session->target_id( ),
								 nwrite );

			fix_ctx->active_sessions[ msg.fd ].reset( );

			ctx->erase_target( msg.fd );
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
	auto ctx = cli.ctx( );
	auto fix_ctx = cli.fix_ctx( );

	int ret = cli.connect( );
	if ( !ret ) {
		return 0;
	}

	for ( auto &[ fd, session ] : cli.fix_ctx( )->active_sessions ) {
		fix_ctx->log->info( "Sending Logon to {}...", session->target_id( ) );

		auto buf = ctx->bufpool.get( );

		fix::fix_writer wr{ buf, ctx->bufpool.obj_size };

		wr.push_header( session->begin_str( ).c_str( ) );
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

		if ( !ctx->out_queue.try_push( message::net_msg_t{ fd, buf, wr.size( ) } ) ) {
			fix_ctx->log->critical( "Failed to push outgoing msg" );
		}
	}

	std::thread( out_queue_consume, std::ref( cli ) ).detach( );

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 2; ++i )
		std::thread( in_queue_consume, std::ref( cli ) ).detach( );

	return cli.run( );
}