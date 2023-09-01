#include "include.h"

#include "tcp_server.h"

struct tcp_handler_t;
using tcp_server = net::tcp_server_impl< 16, 16, 128, 64, tcp_handler_t >;

struct tcp_handler_t {
	std::vector< int > active_clients;
	tcp_server *srv;

	tcp_handler_t( tcp_server *ptr ) : srv{ ptr } { };

	void on_connect( int fd ) {}
	bool on_out( const net::tagged_tcp_msg_t &msg ) {
		return std::ranges::find( active_clients, msg.fd ) != active_clients.end( );
	}

	bool on_read( int fd, char *buf, size_t len ) {
		const std::string_view msg{ buf, len };
		

		if ( std::ranges::find( active_clients, fd ) != active_clients.end( ) ) {
			srv->post( net::tagged_tcp_msg_t{ { buf, len }, 0 } );
			return false;
		}

		if ( msg.find( "/sub" ) != std::string::npos ) {
			active_clients.emplace_back( fd );
			
		}

		return true;
	}

	void on_disconnect( int fd ) {
		auto it = std::ranges::find( active_clients, fd );
		if ( it != active_clients.end( ) ) {
			active_clients.erase( it );
		}
	}
};

int main( ) {
	auto server = std::make_unique< tcp_server >( "TCP" );
	if ( server->bind( "127.0.0.1", "3333" ) != 0 ) {
		spdlog::error( "Failed to bind." );
		return 0;
	}

	server->run( );

	return 0;
}