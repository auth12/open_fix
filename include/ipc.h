#pragma once

#include "tcp_server.h"

namespace ipc {
	struct subscriber_t {
		bool subscribed = false;
	};

	class ipc_server : public net::tcp_server {
	  private:
		std::unordered_map< int, subscriber_t > m_subs;
		details::log_ptr_t m_log;

		static void on_read_cb( ipc_server *ipc, const int fd, char *data, size_t len ) {
			std::string_view buf{ data, len };

			if ( buf.find( "/sub" ) != std::string::npos ) {
				ipc->register_subscriber( fd );
			}
		}

		static void on_disconnect_cb( ipc_server *ipc, const int fd, char *data, size_t len ) {
			ipc->remove_subscriber( fd );
		}

	  public:
		ipc_server( ) : m_log{ details::log::make_sync( "IPC" ) }, net::tcp_server{ "SRV" } {
			m_log->set_level( spdlog::level::debug );
			m_log->debug( "Registering server callbacks..." );
			m_log->set_pattern( LOG_PATTERN );
			register_callback( net::CB_ON_READ, std::bind( ipc_server::on_read_cb, this, std::placeholders::_1,
														   std::placeholders::_2, std::placeholders::_3 ) );
			register_callback( net::CB_ON_DISCONNECT,
							   std::bind( ipc_server::on_disconnect_cb, this, std::placeholders::_1,
										  std::placeholders::_2, std::placeholders::_3 ) );
		}

		void register_subscriber( const int fd ) {
			m_log->info( "FD: {} subscribed to stream", fd );
			m_subs[ fd ].subscribed = true;
		}

		void remove_subscriber( const int fd ) { m_subs.erase( fd ); }

		void post_sub_msg( std::string_view str ) {
			m_log->info( "Sent {}", str );
			post_sub_msg( str.data( ), str.size( ) );
		}

		void post_sub_msg( const char *data, size_t len ) {
			if ( m_subs.empty( ) ) {
				return;
			}

			auto sendbuf = get_buf( );
			if ( !sendbuf ) {
				return;
			}

			memcpy( sendbuf, data, len );

			for ( auto &[ fd, s ] : m_subs ) {
				post( sendbuf, len, fd );
			}

			// release buffer
			post( sendbuf, 0, 0 );
		}
	};
}; // namespace ipc