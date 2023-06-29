#include "include.h"

#include "fix_client.h"

#include <openssl/rand.h>
#include <openssl/sha.h>
#include <config.h>

#include <sched.h>
#include <pthread.h>

void in_queue_consume( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->in_queue.pop( );

		fix::fix_message_t rd{ msg->buf, msg->buf + msg->len };
		auto cur = rd.begin( );
		if ( cur->tag != fix_spec::BeginString ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::BodyLength ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::MsgType ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		if ( cur->val.as_char( ) == 'A' ) {
			log->info( "successfully logged in" );

			auto buf_ = ctx->bufpool.get( );

			auto timestamp = std::chrono::system_clock::now( );

			hffix::message_writer wr{ buf_, buf_ + ctx->bufpool.obj_size( ) };

			wr.push_back_header( "FIX.4.4" );
			wr.push_back_char( fix_spec::MsgType, 'V' );

			wr.push_back_string( fix_spec::TargetCompID, "LMXBLM" );
			wr.push_back_string( fix_spec::SenderCompID, "AshishDigUAT1" );
			wr.push_back_timestamp( fix_spec::SendingTime, timestamp );
			wr.push_back_int( fix_spec::MsgSeqNum, 2 );

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

			ctx->out_queue.push( std::move( data_req ) );
		}

		// add checksum field check

		log->info( "got valid fix message, type: {}, len: {}", cur->val.as_char( ), msg->len );

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

		ctx->bufpool.release( msg->buf );
	}
}

void out_queue_consume( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	while ( !ctx->targets.empty( ) ) {
		auto msg = ctx->out_queue.pop( );

		std::string str{ msg->buf, msg->len };
		for ( auto &c : str ) {
			if ( c == 0x1 ) {
				c = '|';
			}
		}

		log->info( "sending {}", str );

		ssize_t nwrite = net::sock::write( msg->fd, msg->buf, msg->len );
		log->debug( "write to {} returned {}", msg->fd, nwrite );

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
	if ( ret == 0 ) {
		log->error( "no active connections." );
		return 0;
	}

	for ( auto &[ fd, session ] : cli.fix_ctx( )->active_sessions ) {
		auto buf = ctx->bufpool.get( );

		auto timestamp = std::chrono::system_clock::now( );

		hffix::message_writer wr{ buf, buf + ctx->bufpool.obj_size( ) };

		wr.push_back_header( session->begin_str( ).c_str( ) );
		wr.push_back_char( fix_spec::MsgType, 'A' );

		wr.push_back_string( fix_spec::TargetCompID, session->target_id( ) );
		wr.push_back_string( fix_spec::SenderCompID, session->sender_id( ) );
		wr.push_back_timestamp( fix_spec::SendingTime, timestamp );
		wr.push_back_int( fix_spec::MsgSeqNum, 1 );
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

	cli.run( );

	return 0;
}