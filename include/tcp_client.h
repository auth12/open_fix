#pragma once

#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"
#include "errors.h"

#include "tscns.h"

namespace net {
	template < size_t BufSize, size_t BufpoolSize, size_t OutSize, class Handler >
	class tcp_client_impl : public tcp_session {
	  private:
		details::log_ptr_t m_log;
		details::object_pool< char, BufSize, BufpoolSize > m_bufpool;
		atomic_queue::AtomicQueue2< net::tcp_msg_t, OutSize > m_outgoing;

	  protected:
		Handler m_handler;

	  public:
		tcp_client_impl( const std::string_view log_name, bool to_file = false )
			: m_log{ details::log::make_sync( log_name, to_file ) }, m_handler{ } {
			m_log->set_pattern( LOG_PATTERN );
			m_log->set_level( spdlog::level::debug );
		}

		// release a buffer back into the pool, busy waits
		void release_buf( char *buf ) {
			m_log->debug( "Releasing buffer {:x}", uintptr_t( buf ) );
			m_bufpool.release( buf );
		}

		// returns a buffer from the buffer pool, busy waits
		char *get_buf( ) {
			auto ret = m_bufpool.get( );
			m_log->debug( "Got buffer {:x}", uintptr_t( ret ) );
			return ret;
		}

		int connect( const std::string_view host, const std::string_view port ) {
			struct addrinfo hints;
			struct addrinfo *info;

			m_log->info( "Connecting to {}:{}", host, port );

			memset( &hints, 0, sizeof( hints ) );
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			int ret = getaddrinfo( host.data( ), port.data( ), &hints, &info );
			if ( ret != 0 ) {
				m_log->error( "getaddrinfo failed, {}", ret );
				return tcp_error::getaddrinfo_err;
			}

			for ( auto cur = info; cur != nullptr; cur = info->ai_next ) {
				m_fd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
				if ( m_fd < 0 ) {
					continue;
				}

				ret = ::connect( m_fd, cur->ai_addr, cur->ai_addrlen );
				if ( ret < 0 ) {
					close( );
					continue;
				}

				m_log->debug( "Socket {} established connection", m_fd );

				break;
			}

			freeaddrinfo( info );

			if ( m_fd < 0 ) {
				m_log->error( "Failed to connect to {}:{}", host, port );
				return tcp_error::connect_err;
			}

			m_log->debug( "Setting socket {} to nonblock...", m_fd );

			ret = fcntl( m_fd, F_SETFL, O_NONBLOCK );
			if ( ret != 0 ) {
				m_log->error( "Failed to set socket to non block." );
				close( );
				return tcp_error::sock_err;
			}

			m_log->debug( "Setting TCP_NODELAY..." );

			int yes = 1;
			ret = setsockopt( m_fd, IPPROTO_TCP, TCP_NODELAY, ( char * )&yes, sizeof( int ) );
			if ( ret != 0 ) {
				m_log->error( "Failed to set TCP_NODELAY, {}", ret );
				close( );
				return tcp_error::sock_err;
			}

			m_log->info( "Connected to {}:{}", host, port );

			return tcp_error::ok;
		}

		void post( const net::tcp_msg_t &msg ) { m_outgoing.push( msg ); }

		// start polling the file descriptor
		void poll( ) {
			pollfd pfd{ m_fd, POLLIN | POLLHUP | POLLOUT };
			while ( m_fd > 0 ) {
				pfd.revents = 0;

				int nevents = ::poll( &pfd, 1, 1 );
				if ( nevents < 0 ) {
					m_log->error( "Poll error, {}", nevents );
					close( );
					break;
				}

				if ( pfd.revents & POLLHUP ) {
					m_log->warn( "Connection reset." );
					close( );
					break;
				}

				if ( pfd.revents & POLLIN ) {
					m_log->debug( "Buffer pool available elements: {}", m_bufpool.pool_size( ) );
					auto buf = get_buf( );
					if ( !buf ) {
						m_log->warn( "No available buffers in pool." );
						continue;
					}

					int nread = read( buf, m_bufpool.obj_size );
					if ( nread <= 0 ) {
						m_log->error( "Read returned {} on fd: {}, resetting...", nread, m_fd );
						release_buf( buf );
						close( );
						break;
					}

					m_log->debug( "IN -> fd: {}, buf: {:x}, size: {} bytes", m_fd, uintptr_t( buf ), nread );
					m_log->info( "IN -> Read {} bytes", nread );

					if ( m_handler( reinterpret_cast< tcp_session * >( this ), buf, nread ) )
						release_buf( buf );
				}

				if ( pfd.revents & POLLOUT ) {
					net::tcp_msg_t msg{ };
					if ( m_outgoing.try_pop( msg ) ) {
						int ret = write( msg.buf, msg.len );
						if ( ret <= 0 ) {
							m_log->error( "Failed to send message, resetting." );
							close( );
							break;
						}

						m_log->info( "OUT -> {}", std::string_view{ msg.buf, msg.len } );
						m_log->debug( "OUT -> {} bytes written", ret );

						release_buf( msg.buf );
					}
				}
			}
		}
	};
}; // namespace net