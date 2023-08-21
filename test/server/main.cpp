#include "include.h"

#include <tcp_server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct chat_handler;
using chat_server = net::tcp_server_impl< 16, 16, 128, 64, chat_handler >;

struct chat_handler {
	chat_server *server;

	chat_handler( chat_server *ptr ) : server{ ptr } {}

	void on_connect( int fd ) {}

	bool on_read( int fd, char *buf, size_t len ) {
		std::string_view str{ buf, len };

		server->post( net::outgoing_tcp_msg_t{ net::msg_type::broadcast, buf, len } );

		return false;
	}

	void on_disconnect( int fd ) {}
};

int main( ) {
	auto srv = std::make_unique< chat_server >( "TCP" );

	if ( srv->bind( HOST, PORT ) != 0 ) {
		spdlog::error( "Failed to bind" );
		return 0;
	}

	spdlog::info( "Listening on {}:{}", HOST, PORT );

	srv->run( );

	return 0;
};