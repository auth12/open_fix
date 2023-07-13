#include "include.h"

#include "tcp_client.h"

int net::tcp_client::connect( const std::string_view host, const std::string_view port ) {
	struct addrinfo hints;
	struct addrinfo *info;

	m_ctx->log->debug( "Connecting to {}:{}", host, port );

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int sockfd = -1;

	int ret = getaddrinfo( host.data( ), port.data( ), &hints, &info );
	if ( ret != 0 ) {
		m_ctx->log->error( "getaddrinfo failed, code {}", ret );
		return tcp_error::getaddrinfo_err;
	}

	struct addrinfo *cur;
	for ( cur = info; cur != nullptr; cur = info->ai_next ) {
		sockfd = ::socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if ( sockfd < 0 ) {
			sockfd = -1;
			continue;
		}

		ret = ::connect( sockfd, cur->ai_addr, cur->ai_addrlen );
		if ( ret < 0 ) {
			close( sockfd );
			sockfd = -1;
			continue;
		}

		break;
	}

	freeaddrinfo( info );

	if ( sockfd < 0 ) {
		m_ctx->log->debug( "Failed to connect to {}:{}", host, port );
		return tcp_error::connect_err;
	}

	m_ctx->log->debug( "Connected to {}:{}, setting socket to nonblock...", host, port );

	int on = 1;
	ret = ioctl( sockfd, FIONBIO, &on );
	if ( ret < 0 ) {
		m_ctx->log->error( "Failed to set socket to non block." );
		close( sockfd );
		return tcp_error::sock_err;
	}

	m_ctx->log->debug( "Set socket {} to non block.", sockfd );

	m_ctx->targets.emplace_back( pollfd{ sockfd, POLLIN | POLLHUP } );

	return sockfd;
}

void net::tcp_client::run( ) {
	while ( !m_ctx->targets.empty( ) ) {
		int nevents = poll( m_ctx->targets.data( ), m_ctx->targets.size( ), 0 );
		if ( nevents < 0 ) {
			m_ctx->log->error( "Poll error, {}", nevents );
			break;
		}

		if ( nevents == 0 ) {
			continue;
		}

		m_ctx->log->debug( "Captured {} events.", nevents );

		for ( auto it = m_ctx->targets.cbegin( ); it != m_ctx->targets.cend( ); ++it ) {
			if ( nevents <= 0 ) {
				break;
			}

			if ( it->revents == 0 ) {
				continue;
			}

			if ( it->revents & POLLHUP ) {
				--nevents;
				m_ctx->log->warn( "Connection reset by peer, fd: {}", it->fd );

				::close( it->fd );

				m_ctx->targets.erase( it );
				continue;
			}

			if ( it->revents & POLLIN ) {
				--nevents;
				auto buf = m_ctx->bufpool.get( );
				if ( !buf ) {
					m_ctx->log->warn( "No available buffers in pool, postponing fd: {}", it->fd );
					continue;
				}

				int nread = net::read( it->fd, buf, m_ctx->bufpool.obj_size );
				if ( nread <= 0 ) {
					m_ctx->log->warn( "Read returned {} on fd: {}, dropping...", nread, it->fd );

					::close( it->fd );

					m_ctx->targets.erase( it );
					m_ctx->bufpool.release( buf );
					continue;
				}

				if ( nread == EWOULDBLOCK ) {
					m_ctx->log->debug( "Socket fd: {} would block, discarding...", it->fd );
					m_ctx->bufpool.release( buf );
					continue;
				}

				m_ctx->in_queue.push( message::net_msg_t{ it->fd, buf, nread } );
			}
		}
	}
};