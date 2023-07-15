#include "include.h"

#include "tcp_server.h"

int net::tcp_server::bind( const std::string_view host, const std::string_view port ) {
	addrinfo hints, *res;

	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo( host.data( ), port.data( ), &hints, &res );
	if ( ret != 0 ) {
		m_ctx->log->error( "Failed to get address info, ret {}", ret );
		return ret;
	}

	int fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
	if ( fd < 0 ) {
		m_ctx->log->error( "Failed to create socket" );

		freeaddrinfo( res );
		return -1;
	}

	m_ctx->log->debug( "Created socket {}", fd );

	ret = ::bind( fd, res->ai_addr, res->ai_addrlen );
	if ( ret != 0 ) {
		m_ctx->log->error( "Failed to bind." );
		close( fd );

		freeaddrinfo( res );
		return ret;
	}

	freeaddrinfo( res );

	m_ctx->log->debug( "Bound socket {}", fd );

	int on = 1;
	ret = ioctl( fd, FIONBIO, &on );
	if ( ret != 0 ) {
		m_ctx->log->error( "Failed to set listen socket to non-block" );
		return ret;
	}

	m_ctx->log->debug( "Set socket {} to non-block" );

	ret = listen( fd, 128 );
	if ( ret != 0 ) {
		m_ctx->log->error( "failed to listen" );
		close( fd );
		return ret;
	}

	m_ctx->log->debug( "Socket {} listening", fd );

	m_ctx->targets.emplace_back( net::poll_t{ fd, POLLIN } );
	m_listen_session.set_fd( fd );

	return fd;
}

void net::tcp_server::run( ) {
	while ( !m_ctx->targets.empty( ) ) {
		int nevents = poll( ( pollfd * )m_ctx->targets.data( ), m_ctx->targets.size( ), 0 );

		if ( nevents < 0 ) {
			m_ctx->log->error( "Poll error, {}", nevents );
			break;
		}

		if ( nevents == 0 ) {
			continue;
		}

		m_ctx->log->debug( "Captured {} events.", nevents );

		for ( auto it = m_ctx->targets.begin( ); it != m_ctx->targets.end( ); ++it ) {
			if ( !nevents ) {
				break;
			}

			if ( !it->revents ) {
				continue;
			}

			if ( it->revents & POLLHUP ) {
				--nevents;
				m_ctx->log->warn( "Session fd: {} disconnected.", it->session );
				it->session.reset( );

				m_ctx->targets.erase( it );
				continue;
			}

			if ( it->revents & POLLIN ) {
				--nevents;

				if ( it->session == m_listen_session ) {
					int fd = accept( m_listen_session, nullptr, nullptr );
					if ( fd < 0 ) {
						m_ctx->log->error( "Accept failed on new session." );
						continue;
					}

					m_ctx->log->info( "New session established fd: {}", fd );

					m_ctx->targets.emplace_back( poll_t{ fd, POLLIN | POLLHUP } );

					continue;
				}

				auto buf = m_ctx->bufpool.get( );
				if ( !buf ) {
					m_ctx->log->warn( "No available buffers in pool, postponing fd: {}", it->session );
					continue;
				}

				int nread = it->session.read( buf, m_ctx->bufpool.obj_size );
				if ( nread <= 0 ) {
					m_ctx->log->error( "Read returned {} on fd: {}, dropping...", nread, it->session );

					it->session.reset( );

					m_ctx->targets.erase( it );
					m_ctx->bufpool.release( buf );
					continue;
				}

				if ( nread == EWOULDBLOCK ) {
					m_ctx->log->warn( "Socket fd: {} would block, discarding...", it->session );
					m_ctx->bufpool.release( buf );
					continue;
				}

				m_ctx->log->debug( "Read {} bytes from fd: {}", nread, it->session );

				m_ctx->in_queue.push( message::net_msg_t{ it->session, buf, nread } );
			}
		}
	}
}