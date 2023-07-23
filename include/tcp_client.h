#pragma once

#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"
#include "errors.h"

namespace net {
	constexpr int in_queue_size = 256;
	constexpr int out_queue_size = 128;
	constexpr int buffer_size = 512;
	constexpr int bufpool_size = 128;

	template < int InSize, int OutSize, int BufSize, int BufPoolSize > class tcp_client_impl : public tcp_session {
	  private:
		atomic_queue::AtomicQueue2< tcp_msg_t, InSize, true, true, true, true > m_in_queue;
		atomic_queue::AtomicQueue2< tcp_msg_t, OutSize > m_out_queue;

		details::object_pool< char, BufSize, BufPoolSize > m_bufpool;
		details::log_ptr_t m_log;

	  public:
		tcp_client_impl( const std::string_view log_name, bool to_file = false )
			: m_log{ details::log::make_sync( log_name, to_file ) } {
			m_log->set_level( spdlog::level::debug );
			m_log->set_pattern( LOG_PATTERN );
		}

		// returns an incoming message, busy waits
		bool consume_in( tcp_msg_t &msg ) { return m_in_queue.try_pop( msg ); }

		// spawns consumer thread
		template < typename Fn = std::function< bool( tcp_msg_t ) > > void spawn_consumer( Fn &&fn ) {
			std::thread(
				[ this ]( Fn &consume ) {
					while ( m_fd > 0 ) {
						tcp_msg_t msg{ };
						if ( consume_in( msg ) ) {
							if ( consume( msg ) ) {
								release_buf( msg.buf.data( ) );
							}
						}
					}
				},
				std::ref( fn ) )
				.detach( );
		}

		// pushes a message to the outgoing queue, busy waits
		void post( char *buf, size_t len ) { m_out_queue.push( tcp_msg_t{ buf, len } ); }

		// release a buffer back into the pool, busy waits
		void release_buf( char *buf ) { m_bufpool.release( buf ); }

		// returns a buffer from the buffer pool
		char *get_buf( ) { return m_bufpool.get( ); }

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
			if ( ret < 0 ) {
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

		// start polling the file descriptor
		void poll( ) {
			pollfd pfd{ m_fd, POLLIN | POLLOUT | POLLHUP };
			while ( m_fd > 0 ) {
				pfd.revents = 0;

				int nevents = ::poll( &pfd, 1, 0 );
				if ( nevents < 0 ) {
					m_log->error( "Poll error, {}", nevents );
					close( );
					break;
				}

				if ( pfd.revents & POLLOUT ) {
					tcp_msg_t msg{ };
					if ( m_out_queue.try_pop( msg ) ) {
						m_log->debug( "OUT -> {}", msg.buf.data( ) );
						int nwrite = write( msg.buf.data( ), msg.buf.size( ) );
						if ( nwrite <= 0 ) {
							m_log->error( "Write returned {}, resetting...", nwrite );
							close( );
						}

						release_buf( msg.buf.data( ) );
					}

					if ( m_fd < 0 )
						break;
				}

				if ( pfd.revents & POLLHUP ) {
					m_log->warn( "Connection reset on fd: {}", m_fd );
					close( );

					break;
				}

				if ( pfd.revents & POLLIN ) {
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

					m_in_queue.push( tcp_msg_t{ buf, ( size_t )nread } );
				}
			}
		}
	};

	using tcp_client = tcp_client_impl< in_queue_size, out_queue_size, buffer_size, bufpool_size >;
}; // namespace net