#include "include.h"

#include "fix_client.h"

#include <openssl/rand.h>
#include <openssl/sha.h>
#include <config.h>

#include <sched.h>
#include <pthread.h>

void consumer( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	while ( 1 ) {
		auto msg = ctx->msg_queue.pop( );

		std::string_view buf{ msg->buf, msg->len };
		const int fd = msg->session->fd( );

		log->info( buf );

		fix::fix_message_t rd{ buf };
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

		// add checksum field check

		log->info( "fix message, type: {}, len: {}, fd: {}", cur->val.as_str( ), msg->len, fd );

		++cur;

		for ( ; cur < rd.end( ); ++cur ) {
			if ( cur->tag == fix_spec::AvgPx ) {
				log->info( "got px update: {}", cur->val.as_float( ) );
			}
		}

		ctx->bufpool.release( msg->buf );
	}
}

void on_con_cb( net::tcp_session *session ) {
	const auto timestamp = std::chrono::system_clock::now( );

	static char out_buf[ 1024 ];
	hffix::message_writer wr{ out_buf, sizeof( out_buf ) };
	wr.push_back_header( "FIX.4.4" );
	wr.push_back_char( fix_spec::MsgType, 'A' );
	wr.push_back_string( fix_spec::TargetCompID, "LMXBLM" );
	wr.push_back_string( fix_spec::SenderCompID, "AshishDigUAT1" );
	wr.push_back_timestamp( fix_spec::SendingTime, timestamp );
	wr.push_back_int( fix_spec::MsgSeqNum, 1 );
	wr.push_back_int( fix_spec::EncryptMethod, 0 );

	wr.push_back_string( fix_spec::Username, "AshishDigUAT1" );
	wr.push_back_string( fix_spec::Password, "AshishDigUAT1" );
	wr.push_back_trailer( );

	std::string_view str{ out_buf, wr.message_size( ) };
	spdlog::info( "{}", spdlog::to_hex( str ) );

	session->write( ( void * )out_buf, wr.message_size( ) );
}

int main( ) {
	config::cli_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "failed to parse client config file" );
		return 0;
	}

	fix::fix_client cli( "tcp_client", false );
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	cli.register_callback( net::ON_CONNECT, on_con_cb );

	for ( auto &s : cfg.targets ) {
		const auto cur = s.ip.find( ':' );
		if ( cur == std::string::npos ) {
			log->warn( "invalid address specified in config file" );
			continue;
		}
		auto ip = s.ip.substr( 0, cur );
		auto port = s.ip.substr( cur + 1 );

		log->info( "attempting to connect to {}:{}", ip, port );

		int ret = cli.connect( ip, port );
		if ( ret != 0 ) {
			log->error( "failed to connect to {}:{}, ret: {}", ip, port, ret );
		}
	};

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 1; ++i )
		std::thread( consumer, std::ref( cli ) ).detach( );

	int ret = cli.run( );

	return ret;
}