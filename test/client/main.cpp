#include "include.h"

#include <client.h>

#include <fix_parse.h>

#include <config.h>


void on_timer_cb( uv_timer_t *handle ) {
	auto cli = ( net::tcp_client * )handle->loop->data;
	auto &ctx = cli->ctx( );
	auto &log = cli->log( );

	static char buf[ 2048 ];

	/*if ( ctx->targets.empty( ) ) {
		log->warn( "no target left." );
		uv_timer_stop( handle );
		return;
	}

	for ( auto &srv : ctx->targets ) {
		if ( srv.state( ) < net::Idle ) {
			continue;
		}

		fix::fix_writer_t wr{ buf, sizeof( buf ) };
		wr.push_header( fmt::format( "FIX.{}.{}", 4, 4 ) );
		wr.push_int( fix_spec::MsgSeqNum, 1 );

		wr.push_trailer( );

		srv.write( buf, wr.cur_pos );

		memset( buf, 0, sizeof( buf ) );
	}*/
}

int main( ) {
	config::cli_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "failed to parse client config file" );
		return 0;
	}

	net::tcp_client cli( "tcp_client", false, 1024 );
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
			log->error( "failed to connect to {}:{}.", ip, port );
			continue;
		}

		log->info( "connected." );
	};
	
	uv_timer_t t;
	uv_timer_init( cli.loop( ), &t );
	uv_timer_start( &t, on_timer_cb, 0, 100 );

	return cli.run( );
}