#pragma once

#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"
#include "errors.h"

#include "tscns.h"

namespace net {
	constexpr int in_queue_size = 256;
	constexpr int out_queue_size = 128;
	constexpr int buffer_size = 512;
	constexpr int bufpool_size = 128;

	template < int InSize, int OutSize, int BufSize, int BufPoolSize, typename MsgType, bool Serial, class ReadHandler >
	class tcp_client_impl : public tcp_session {
	  private:
		atomic_queue::AtomicQueue2< MsgType, InSize, true, true, false, true > m_in_queue;
		atomic_queue::AtomicQueue2< MsgType, OutSize > m_out_queue;
		details::object_pool< char, BufSize, BufPoolSize > m_bufpool;
		details::log_ptr_t m_log;

		TSCNS m_clock;

	  public:
		tcp_client_impl( const std::string_view log_name, bool to_file = false )
			: m_log{ details::log::make_sync( log_name, to_file ) } {
			m_log->set_pattern( LOG_PATTERN );
			m_log->set_level( spdlog::level::debug );

			m_clock.init( );
			m_clock.calibrate( );
		}

		TSCNS *clock( ) { return &m_clock; }

		// pushes a message to the outgoing queue
		void post( const MsgType &msg ) { m_out_queue.push( msg ); }

		// returns false if queue is empty
		bool consume_in( MsgType &msg ) { return m_in_queue.try_pop( msg ); }

		// release a buffer back into the pool, busy waits
		void release_buf( char *buf ) {
			SPDLOG_LOGGER_DEBUG( m_log, "Releasing buffer {:x}", uintptr_t( buf ) );
			m_bufpool.release( buf );
		}

		// returns a buffer from the buffer pool
		char *get_buf( ) {
			auto ret = m_bufpool.get( );
			SPDLOG_LOGGER_DEBUG( m_log, "Got buffer {:x}", uintptr_t( ret ) );
			return ret;
		}

		int connect( const std::string_view host, const std::string_view port ) {
			struct addrinfo hints;
			struct addrinfo *info;

			SPDLOG_LOGGER_INFO( m_log, "Connecting to {}:{}", host, port );

			memset( &hints, 0, sizeof( hints ) );
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			int ret = getaddrinfo( host.data( ), port.data( ), &hints, &info );
			if ( ret != 0 ) {
				SPDLOG_LOGGER_ERROR( m_log, "getaddrinfo failed, {}", ret );
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

				SPDLOG_LOGGER_DEBUG( m_log, "Socket {} established connection", m_fd );

				break;
			}

			freeaddrinfo( info );

			if ( m_fd < 0 ) {
				SPDLOG_LOGGER_ERROR( m_log, "Failed to connect to {}:{}", host, port );
				return tcp_error::connect_err;
			}

			SPDLOG_LOGGER_DEBUG( m_log, "Setting socket {} to nonblock...", m_fd );

			ret = fcntl( m_fd, F_SETFL, O_NONBLOCK );
			if ( ret != 0 ) {
				SPDLOG_LOGGER_ERROR( m_log, "Failed to set socket to non block." );
				close( );
				return tcp_error::sock_err;
			}

			SPDLOG_LOGGER_DEBUG( m_log, "Setting TCP_NODELAY..." );

			int yes = 1;
			ret = setsockopt( m_fd, IPPROTO_TCP, TCP_NODELAY, ( char * )&yes, sizeof( int ) );
			if ( ret != 0 ) {
				SPDLOG_LOGGER_ERROR( m_log, "Failed to set TCP_NODELAY, {}", ret );
				close( );
				return tcp_error::sock_err;
			}

			SPDLOG_LOGGER_INFO( m_log, "Connected to {}:{}", host, port );

			return tcp_error::ok;
		}

		// start polling the file descriptor
		void poll( ) {
			pollfd pfd{ m_fd, POLLIN | POLLHUP | POLLOUT };
			while ( m_fd > 0 ) {
				pfd.revents = 0;

				int nevents = ::poll( &pfd, 1, 1 );
				if ( nevents < 0 ) {
					SPDLOG_LOGGER_ERROR( m_log, "Poll error, {}", nevents );
					close( );
					break;
				}

				if ( pfd.revents & POLLHUP ) {
					SPDLOG_LOGGER_WARN( m_log, "Connection reset." );
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
						if ( errno == EWOULDBLOCK ) {
							SPDLOG_LOGGER_ERROR( m_log, "Socket would block, discarding buf..." );
							release_buf( buf );
							continue;
						}

						SPDLOG_LOGGER_ERROR( m_log, "Read returned {} on fd: {}, resetting...", nread, m_fd );
						release_buf( buf );
						close( );
						break;
					}

					SPDLOG_LOGGER_DEBUG( m_log, "IN -> fd: {}, buf: {:x}, size: {} bytes", m_fd, uintptr_t( buf ),
										 nread );
					SPDLOG_LOGGER_INFO( m_log, "IN -> Read {} bytes", nread );
					const auto ts = m_clock.rdtsc( );
					if constexpr ( Serial ) {
						if ( ReadHandler( )( MsgType{ buf, ( size_t )nread, ts } ) )
							release_buf( buf );
					} else {
						m_in_queue.push( MsgType{ buf, ( size_t )nread, ts } );
					}
				}

				if ( pfd.revents & POLLOUT ) {
					MsgType msg{ };
					if ( m_out_queue.try_pop( msg ) ) {
						std::string_view buf{ msg.buf, msg.len };

						int nwrite = write( buf );
						if ( nwrite <= 0 ) {
							SPDLOG_LOGGER_ERROR( m_log, "Write returned {}, resetting...", nwrite );
							close( );
						}

						SPDLOG_LOGGER_DEBUG( m_log, "OUT -> {}", buf );
						SPDLOG_LOGGER_INFO( m_log, "OUT -> Sent {} bytes\n\t{}", nwrite, buf );
						release_buf( msg.buf );
					}
				}
			}
		}
	};

	using tcp_client =
		tcp_client_impl< in_queue_size, out_queue_size, buffer_size, bufpool_size, tcp_msg_t, false, std::void_t<> >;
}; // namespace net