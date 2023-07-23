#include "include.h"

#include <tcp_server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

void on_read_cb( std::unique_ptr< net::tcp_server > &srv, const int fd, char *buf, size_t len ) {
	spdlog::info( "Got msg from fd {}", fd );
	auto send_buf = srv->get_buf( );
	if ( !send_buf ) {
		spdlog::error( "No available buffers" );
		return;
	}

	memcpy( send_buf, buf, len );

	srv->post( send_buf, len );
}

int main( ) {
	auto srv = std::make_unique< net::tcp_server >( "TCP" );

	if ( srv->bind( HOST, PORT ) != 0 ) {
		spdlog::error( "Failed to bind" );
		return 0;
	}

	spdlog::info( "Listening on {}:{}", HOST, PORT );

	srv->register_callback( net::CB_ON_READ, std::bind( on_read_cb, std::ref( srv ), std::placeholders::_1,
													   std::placeholders::_2, std::placeholders::_3 ) );

	

	return srv->run( );
};