#pragma once

#include "tcp_server.h"

namespace ipc {
	struct ipc_handler_t;
	using ipc_server = net::tcp_server_impl< 16, 16, 128, 64, ipc_handler_t >;

	struct ipc_handler_t {
		ipc_server *server;
		std::vector< int > m_subs;

		ipc_handler_t( ipc_server *ptr ) : server{ ptr } {}

		void on_connect( int fd ) {}

		bool on_read( int fd, char *buf, size_t len ) {
			std::string_view str{ buf, len };

			if ( str.find( "/sub" ) != std::string::npos ) {
				m_subs.emplace_back( fd );
			}

			return true;
		}

		void on_disconnect( int fd ) {
			auto it = std::ranges::find( m_subs, fd );
			if ( it != m_subs.end( ) ) {
				m_subs.erase( it );
			}
		}
	};
}; // namespace ipc