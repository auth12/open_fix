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

	while ( cli.state( ) == net::ONLINE ) {
		auto msg = ctx->msg_queue.pop( );

		std::string_view buf{ msg->buf, msg->len };

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

		log->info( "fix message, type: {}, len: {}", cur->val.as_str( ), msg->len );

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

		int ret = cli.try_connect( ip, port );
	};

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 1; ++i )
		std::thread( consumer, std::ref( cli ) ).detach( );

	cli.run( );

	return 0;
}