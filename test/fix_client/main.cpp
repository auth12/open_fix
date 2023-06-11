#include "include.h"

#include "fix_client.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

void consumer( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );
	auto &queue = cli.msg_queue( );

	while ( 1 ) {
		auto msg = queue.pop( );
		if ( !msg->buf ) {
			log->critical( "invalid buffer in queue." );
			continue;
		}

		std::string_view buf{ msg->buf, msg->len };

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

		log->info( "fix message, type: {}, len: {}, fd: {}", cur->val.as_str( ), msg->len, msg->session->fd( ) );

		++cur;

		for ( ; cur < rd.end( ); ++cur ) {
			if ( cur->tag == fix_spec::AvgPx ) {
				log->info( "got px update: {}", cur->val.as_float( ) );
			}
		}

		auto server_session = msg->session;

		if ( server_session->state( ) < net::session_state::Idle ) {
			ctx->bufpool.release( msg->buf );
			continue;
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

	fix::fix_client cli( "tcp_client", false );
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	for ( auto &s : cfg.targets ) {
		const auto cur = s.ip.find( ':' );
		if ( cur == std::string::npos ) {
			log->warn( "invalid address specified in config file" );
			continue;
		}
		auto ip = s.ip.substr( 0, cur );
		auto port = s.ip.substr( cur + 1 );

		log->info( "attempting to connect to {}:{}", ip, port );

		int ret = cli.connect( ip, atoi( port.data( ) ) );
		if ( ret != 0 ) {
			log->error( "failed to connect to {}:{}, ret: {}", ip, port, ret );
		}
	};

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 1; ++i )
		std::thread( consumer, std::ref( cli ) ).detach( );

	int ret = cli.run( );

	return ret;
}