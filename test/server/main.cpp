#include "include.h"

#include <tcp_server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

void consumer( net::tcp_server &srv ) {
	auto &ctx = srv.ctx( );

	while ( 1 ) {
		auto msg = ctx->in_queue.pop( );

		auto session = std::find( ctx->targets.begin( ), ctx->targets.end( ), msg.fd );

		std::string_view buf{ msg.buf, msg.len };
		
		ctx->log->info( "IN: {}, fd: {}", buf, msg.fd );

		ctx->bufpool.release( msg.buf );
	}
}

int main( ) {
	net::tcp_server srv( "tcp_server", false );
	auto &ctx = srv.ctx( );
	
	if ( srv.bind( HOST, PORT ) < 0 ) {
		ctx->log->error( "Failed to bind" );
		return 0;
	}

	ctx->log->info( "Listening on {}:{}", HOST, PORT );

	std::thread( consumer, std::ref( srv ) ).detach( );

	srv.run( );

	return 0;
};