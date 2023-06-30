#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

void in_queue_consume( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &fix_ctx = cli.fix_ctx( );
	auto &log = cli.log( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->in_queue.pop( );
		if ( !msg ) {
			log->critical( "popped invalid message ptr" );
			break;
		}

		// copy locally
		std::string buf{ msg->buf, msg->len };
		ctx->bufpool.release( msg->buf );

		log->debug( "IN -> {}", buf );

		auto session = fix_ctx->active_sessions[ msg->fd ];

		fix::fix_message_t rd{ buf.data( ), buf.size( ) };
		auto cur = rd.begin( );
		if ( cur->tag != fix_spec::BeginString ) {
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::BodyLength ) {
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::MsgType ) {
			continue;
		}

		char msg_type = cur->val.as_char( );

		if ( ( msg_type == '0' || msg_type == '1' ) && session->state( ) == fix::LoggedIn ) {

			auto buf_ = ctx->bufpool.get( );

			hffix::message_writer wr{ buf_, buf_ + ctx->bufpool.obj_size( ) };

			wr.push_back_header( session->begin_str( ).c_str( ) );
			wr.push_back_char( fix_spec::MsgType, '0' );

			wr.push_back_string( fix_spec::TargetCompID, session->target_id( ) );
			wr.push_back_string( fix_spec::SenderCompID, session->sender_id( ) );

			wr.push_back_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
			wr.push_back_int( fix_spec::MsgSeqNum, session->next_seq( ) );
			wr.push_back_trailer( );

			auto data_req = message::make_msg( msg->fd, buf_, wr.message_size( ) );

			ctx->out_queue.push( std::move( data_req ) );
			continue;
		}

		if ( session->state( ) == fix::AwaitingLogon ) {
			if ( msg_type == 'A' ) {
				session->set_state( fix::LoggedIn );
				log->info( "session {}:{} logged in.", session->target_id( ), session->sender_id( ) );

				auto buf_ = ctx->bufpool.get( );

				hffix::message_writer wr{ buf_, buf_ + ctx->bufpool.obj_size( ) };

				wr.push_back_header( session->begin_str( ).c_str( ) );
				wr.push_back_char( fix_spec::MsgType, '1' );

				wr.push_back_string( fix_spec::TargetCompID, session->target_id( ) );
				wr.push_back_string( fix_spec::SenderCompID, session->sender_id( ) );
				wr.push_back_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_back_int( fix_spec::MsgSeqNum, session->next_seq( ) );

				wr.push_back_string( fix_spec::TestReqID, "test_req_1" );

				wr.push_back_trailer( );

				auto data_req = message::make_msg( msg->fd, buf_, wr.message_size( ) );

				ctx->out_queue.push( std::move( data_req ) );

				/*log->info( "requesting market data..." );

				auto buf_ = ctx->bufpool.get( );

				hffix::message_writer wr{ buf_, buf_ + ctx->bufpool.obj_size( ) };

				wr.push_back_header( session->begin_str( ).c_str( ) );
				wr.push_back_char( fix_spec::MsgType, 'V' );

				wr.push_back_string( fix_spec::TargetCompID, session->target_id( ) );
				wr.push_back_string( fix_spec::SenderCompID, session->sender_id( ) );

				wr.push_back_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
				wr.push_back_int( fix_spec::MsgSeqNum, session->next_seq( ) );

				wr.push_back_string( fix_spec::MDReqID, "test_req_1" );
				wr.push_back_char( fix_spec::SubscriptionRequestType, '1' );
				wr.push_back_int( fix_spec::MarketDepth, 1 );

				wr.push_back_int( fix_spec::NoMDEntryTypes, 2 );
				wr.push_back_char( fix_spec::MDEntryType, '0' );
				wr.push_back_char( fix_spec::MDEntryType, '1' );

				wr.push_back_int( fix_spec::NoRelatedSym, 1 );
				wr.push_back_string( fix_spec::SecurityID, "5002" );
				wr.push_back_char( fix_spec::SecurityIDSource, '8' );

				wr.push_back_trailer( );

				auto data_req = message::make_msg( msg->fd, buf_, wr.message_size( ) );

				ctx->out_queue.push( std::move( data_req ) );*/
				continue;
			}

			log->warn( "session {}:{} didn't get expected logon confirmation", session->target_id( ),
					   session->sender_id( ) );
		}

		if ( cur->val.as_char( ) == 'W' ) {
			for ( ; cur < rd.end( ); ++cur ) {
				if ( cur->tag == fix_spec::MDEntryPx ) {
					log->info( "got px update: {}", cur->val.as_float( ) );
				}

				if ( cur->tag == fix_spec::SendingTime ) {
					log->info( "time :{}", cur->val.as_str( ) );
				}
			}
		}
	}
}

void out_queue_consume( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->out_queue.pop( );

		auto s = ctx->targets[ msg->fd ];

		std::string str{ msg->buf, msg->len };
		for ( auto &c : str ) {
			if ( c == 0x1 ) {
				c = '|';
			}
		}

		log->debug( "OUT -> {}", str );

		ssize_t nwrite = send( msg->fd, msg->buf, msg->len, 0 );
		if( nwrite <= 0 ) {
			s->reset( );
			s->poll_stop( );
			log->error( "send to {} returned {}, resetting...", msg->fd, nwrite );
		}

		ctx->bufpool.release( msg->buf );
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
	auto &log = cli.log( );

	int ret = cli.connect( );
	if ( !ret ) {
		return 0;
	}

	for ( auto &[ fd, session ] : cli.fix_ctx( )->active_sessions ) {
		auto buf = ctx->bufpool.get( );

		hffix::message_writer wr{ buf, buf + ctx->bufpool.obj_size( ) };

		wr.push_back_header( session->begin_str( ).c_str( ) );
		wr.push_back_char( fix_spec::MsgType, 'A' );

		wr.push_back_string( fix_spec::TargetCompID, session->target_id( ) );
		wr.push_back_string( fix_spec::SenderCompID, session->sender_id( ) );
		wr.push_back_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
		wr.push_back_int( fix_spec::MsgSeqNum, session->next_seq( ) );
		wr.push_back_char( fix_spec::ResetSeqNumFlag, 'Y' );
		wr.push_back_int( fix_spec::EncryptMethod, 0 );
		wr.push_back_int( fix_spec::HeartBtInt, 30 );

		wr.push_back_string( fix_spec::Username, session->sender_id( ) );
		wr.push_back_string( fix_spec::Password, session->sender_id( ) );
		wr.push_back_trailer( );

		auto msg = message::make_msg( fd, buf, wr.message_size( ) );

		ctx->out_queue.push( std::move( msg ) );
	}

	std::thread( out_queue_consume, std::ref( cli ) ).detach( );

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 2; ++i )
		std::thread( in_queue_consume, std::ref( cli ) ).detach( );

	return cli.run( );
}