#pragma once

#include "errors.h"
#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"

namespace net {
	static constexpr int server_bufpool_elements = 256;
	static constexpr int server_buf_size = 1024;
	static constexpr int max_clients = 64;

	using cb_fn_t = std::function< void( const int, char *, size_t ) >;

	enum server_callback_type : int { CB_ON_BIND = 0, CB_ON_CONNECT, CB_ON_READ, CB_ON_DISCONNECT, CB_MAX };

	static bool operator==( const pollfd &pfd, const int fd ) { return pfd.fd == fd; }

	template < int MaxClients, int OutSize, int BufSize, int BufPoolSize > class tcp_server_impl : public tcp_session {
	  private:
		details::log_ptr_t m_log;

		std::vector< pollfd > m_pollfds;

		details::object_pool< char, BufSize, BufPoolSize > m_bufpool;
		atomic_queue::AtomicQueue2< tagged_tcp_msg_t, OutSize, true, true, true, false > m_out_queue;

		std::unordered_map< int, cb_fn_t > m_callbacks;

	  public:
		tcp_server_impl( const std::string_view log_name, const bool to_file = false )
			: m_log{ details::log::make_sync( log_name, to_file ) } {
			m_log->set_level( spdlog::level::debug );
			m_log->set_pattern( LOG_PATTERN );
		}

		// posts a message to a client
		// set fd to 0 to send to all, specify fd for a specific client,
		// if fd is specified, post a message with len 0 and fd 0 to release buffer used,
		// buffer is not automatically released back to pool for single fd send
		void post( char *buf, const size_t len, int fd = 0 ) {
			m_out_queue.push( tagged_tcp_msg_t{ { buf, len }, fd } );
		}

		// get a buffer from the buffer pool
		char *get_buf( ) { return m_bufpool.get( ); }

		// releases a buffer back to buffer pool
		void release_buf( char *buf ) {
			// m_log->debug( "Releasing buffer {:x}", uintptr_t( buf ) );
			m_bufpool.release( buf );
		}

		// registers a callback, CB_ON_BIND, CB_ON_CONNECT, CB_ON_READ, CB_ON_DISCONNECT
		template < typename Fn = cb_fn_t > void register_callback( int type, const Fn &&fn ) {
			if ( type > CB_MAX or type < CB_ON_BIND ) {
				m_log->warn( "Invalid callback type." );
				return;
			}

			m_callbacks[ type ] = std::move( fn );
		}

		int bind( const std::string_view host, const std::string_view port ) {
			addrinfo hints, *res;

			memset( &hints, 0, sizeof hints );
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			int ret = getaddrinfo( host.data( ), port.data( ), &hints, &res );
			if ( ret != 0 ) {
				m_log->error( "Failed to get address info, ret {}", ret );
				return ret;
			}

			m_fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
			if ( m_fd < 0 ) {
				m_log->error( "Failed to create socket" );

				freeaddrinfo( res );
				return -1;
			}

			m_log->debug( "Created socket {}", m_fd );

			ret = ::bind( m_fd, res->ai_addr, res->ai_addrlen );
			if ( ret != 0 ) {
				m_log->error( "Failed to bind." );
				close( );

				freeaddrinfo( res );
				return ret;
			}

			freeaddrinfo( res );

			m_log->debug( "Bound listen socket {} to {}:{}", m_fd, host, port );

			m_log->debug( "Setting O_NONBLOCK..." );

			/*ret = fcntl( m_fd, F_SETFL, O_NONBLOCK );
			if ( ret < 0 ) {
				m_log->error( "Failed to set socket to non block." );
				close( );
				return ret;
			}*/

			m_log->debug( "Setting TCP_NODELAY..." );

			int yes = 1;
			ret = setsockopt( m_fd, IPPROTO_TCP, TCP_NODELAY, ( char * )&yes, sizeof( int ) );
			if ( ret != 0 ) {
				m_log->error( "Failed to set TCP_NODELAY, {}", ret );
				close( );
				return ret;
			}

			ret = listen( m_fd, MaxClients );
			if ( ret != 0 ) {
				m_log->error( "Failed to listen" );
				close( );
				return ret;
			}

			m_log->info( "Listening on {}:{}", host, port );

			m_pollfds.emplace_back( pollfd{ m_fd, POLLIN } );

			return 0;
		}

		int run( ) {
			int ret = 0;
			while ( !m_pollfds.empty( ) ) {
				int nevents = poll( &m_pollfds[ 0 ], m_pollfds.size( ), 0 );
				if ( nevents < 0 ) {
					m_log->error( "Poll error, {}", nevents );
					break;
				}

				if ( nevents == 0 ) {
					continue;
				}

				for ( auto it = m_pollfds.begin( ); it < m_pollfds.end( ); ++it ) {
					if ( it->revents & POLLHUP ) {
						m_log->warn( "Session fd: {} disconnected.", it->fd );

						if ( m_callbacks.contains( CB_ON_DISCONNECT ) ) {
							m_callbacks[ CB_ON_DISCONNECT ]( it->fd, nullptr, 0 );
						}

						::close( it->fd );
						m_pollfds.erase( it );

						break;
					}

					if ( it->revents & POLLOUT ) {
						if ( !m_out_queue.was_empty( ) ) {
							auto msg = m_out_queue.pop( );

							if ( msg.fd == 0 ) {
								if ( msg.len == 0 ) {
									release_buf( msg.buf );
									break;
								}

								for ( auto &c : m_pollfds ) {
									if ( c.fd == m_fd )
										continue;

									int nwrite = net::write( c.fd, msg.buf, msg.len );
									if ( nwrite <= 0 ) {
										m_log->error( "Failed to write to fd: {}, dropping...", c.fd );
										::close( c.fd );
										m_pollfds.erase( it );
									}
								}

								release_buf( msg.buf );
								break;
							} else {
								int nwrite = net::write( msg.fd, msg.buf, msg.len );
								if ( nwrite <= 0 ) {
									m_log->error( "Failed to write to fd: {}, dropping...", msg.fd );
									::close( msg.fd );
									m_pollfds.erase( it );
								}
							}
						}
					}

					if ( it->revents & POLLIN ) {
						if ( it->fd == m_fd ) {
							int fd = accept( m_fd, nullptr, nullptr );
							if ( fd < 0 ) {
								m_log->error( "Accept failed on new session." );
								continue;
							}

							m_log->info( "New session established fd: {}", fd );

							if ( m_callbacks.contains( CB_ON_CONNECT ) ) {
								m_callbacks[ CB_ON_CONNECT ]( it->fd, nullptr, 0 );
							}

							m_pollfds.emplace_back( pollfd{ fd, POLLIN | POLLHUP | POLLOUT } );

							break;
						}

						auto buf = get_buf( );
						if ( !buf ) {
							m_log->warn( "No available buffers in pool, postponing fd: {}", it->fd );
							continue;
						}

						int nread = net::read( it->fd, buf, BufSize );
						if ( nread <= 0 ) {
							m_log->error( "Read returned {} on fd: {}, dropping...", nread, it->fd );

							::close( it->fd );
							m_pollfds.erase( it );

							release_buf( buf );
							break;
						}

						m_log->debug( "IN -> fd: {}, buf: {:x}, len: {}", it->fd, uintptr_t( buf ), nread );
						m_log->info( "Read {} bytes from fd: {}", nread, it->fd );

						if ( m_callbacks.contains( CB_ON_READ ) ) {
							m_callbacks[ CB_ON_READ ]( it->fd, buf, nread );
						}

						++ret;

						release_buf( buf );
					}
				}
			}

			return ret;
		}
	};

	using tcp_server = tcp_server_impl< max_clients, 256, 512, 128 >;
}; // namespace net