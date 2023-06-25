#include "include.h"

#include "client.h"

int net::tcp_client::try_connect( const std::string_view host, const std::string_view port ) {
	struct addrinfo hints;
	struct addrinfo *info;

	m_state = CONNECTING;

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int sockfd = -1;
	int ret = getaddrinfo( host.data( ), port.data( ), &hints, &info );
	if ( ret != 0 ) {
		return ret;
	}

	struct addrinfo *cur;
	for ( cur = info; cur != nullptr; cur = info->ai_next ) {
		sockfd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if ( sockfd == -1 ) {
			continue;
		}

		m_log->info( "cur sock: {}, socktype {}, protocol {}", sockfd, cur->ai_socktype, cur->ai_protocol );
		ret = connect( sockfd, cur->ai_addr, cur->ai_addrlen );
		if ( ret == -1 ) {
			close( sockfd );
			continue;
		}

		break;
	}

	if ( !cur ) {
		freeaddrinfo( info );
		m_log->error( "connection to {}:{} failed.", host, port );
		return -1;
	}

	freeaddrinfo( info );

	ret = fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) | O_NONBLOCK );
	if ( ret != 0 ) {
		m_log->warn( "failed to set socket to non block." );
		close( sockfd );
		return -1;
	}

	m_log->info( "connected to {}:{}", host, port );

	m_state = ONLINE;

	m_ctx->targets.emplace_back( pollfd{ sockfd, POLLIN | POLLHUP | POLLOUT } );

	return sockfd;
}

void net::tcp_client::run( ) {
	while ( state( ) == ONLINE ) {
		int nevents = poll( m_ctx->targets.data( ), m_ctx->targets.size( ), 0 );
		if ( nevents < 0 ) {
			m_log->error( "poll error" );
			break;
		}

		if ( m_ctx->targets.empty( ) ) {
			m_state.store( OFFLINE, std::memory_order_release );
			break;
		}

		if ( nevents == 0 ) {
			continue;
		}

		for ( auto it = m_ctx->targets.begin( ); it < m_ctx->targets.end( ); ++it ) {
			if ( it->revents & POLLHUP ) {
				m_log->warn( "fd {} disconnected.", it->fd );

				m_ctx->remove_target( it );
				continue;
			}

			if ( it->revents & POLLOUT ) {
				
			}

			if ( it->revents & POLLIN ) {
				char *buf = m_ctx->bufpool.get( );
				if ( !buf ) {
					m_log->warn( "no available buffers" );
					continue;
				}

				ssize_t nread = recv( it->fd, buf, m_ctx->bufpool.obj_size( ), 0 );
				if ( nread == EWOULDBLOCK ) {
					continue;
				}

				if ( nread <= 0 ) {
					m_log->warn( "read returned {} on fd: {}", nread, it->fd );

					m_ctx->remove_target( it );
					continue;
				}

				auto msg = std::make_unique< message::net_msg_t >( );
				msg->buf = buf;
				msg->len = nread;
				msg->session = nullptr;

				m_ctx->msg_queue.push( std::move( msg ) );
			}
		}
	}
}