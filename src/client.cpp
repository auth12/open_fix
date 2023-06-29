#include "include.h"

#include "client.h"

int net::tcp_client::try_connect( const std::string_view host, const std::string_view port ) {
	struct addrinfo hints;
	struct addrinfo *info;
	
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int sockfd = -1;

	int ret = getaddrinfo( host.data( ), port.data( ), &hints, &info );
	if ( ret != 0 ) {
		m_log->error( "getaddrinfo failed, code {}", ret );
		return -1;
	}

	struct addrinfo *cur;
	for ( cur = info; cur != nullptr; cur = info->ai_next ) {
		sockfd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if ( sockfd < 0 ) {
			continue;
		}

		ret = connect( sockfd, cur->ai_addr, cur->ai_addrlen );
		if ( ret < 0 ) {
			sock::reset( sockfd );
			sockfd = -1;
			continue;
		}

		break;
	}

	freeaddrinfo( info );

	if ( sockfd < 0 ) {
		m_log->error( "connection to {}:{} failed.", host, port );
		return -1;
	}

	ret = fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) | O_NONBLOCK );
	if ( ret != 0 ) {
		m_log->warn( "failed to set socket to non block." );
		sock::reset( sockfd );
		return -1;
	}

	m_ctx->targets.emplace_back( pollfd{ sockfd, POLLIN | POLLHUP | POLLOUT } );

	return sockfd;
}

bool net::tcp_client::on_poll( const pollfd &it ) {
	if ( it.revents & POLLHUP ) {
		m_log->warn( "fd {} disconnected.", it.fd );
		return true;
	}

	/*if ( it.revents & POLLOUT ) {
		auto &q = m_ctx->out_queue[ it.fd ];
		while ( !q.empty( ) ) { // async me
			std::unique_ptr<
			q.pop( );

			auto ret = sock::write( it.fd, msg->buf, msg->len );
			m_log->debug( "write on {} returned {}", it.fd, ret );

			std::string str{ msg->buf, msg->len };
			for ( auto &c : str ) {
				if ( c == 0x1 ) {
					c = '|';
				}
			}

			m_log->info( "sending {}", str );

			m_ctx->bufpool.release( msg->buf );
		}
	}*/

	if ( it.revents & POLLIN ) {
		char *buf = m_ctx->bufpool.get( );
		if ( !buf ) {
			m_log->warn( "no available buffers" );
			return false;
		}

		ssize_t nread = sock::read( it.fd, buf, m_ctx->bufpool.obj_size( ) );
		if ( nread == EWOULDBLOCK ) {
			m_ctx->bufpool.release( buf );
			return false;
		}

		if ( nread <= 0 ) {
			m_log->warn( "read returned {} on fd: {}", nread, it.fd );
			m_ctx->bufpool.release( buf );
			return true;
		}

		auto msg = std::make_unique< message::net_msg_t >( );
		msg->buf = buf;
		msg->len = nread;
		msg->fd = it.fd;

		m_ctx->in_queue.push( std::move( msg ) );
	}

	return false;
}

void net::tcp_client::run( ) {
	while ( !m_ctx->targets.empty( ) ) {
		int nevents = poll( m_ctx->targets.data( ), m_ctx->targets.size( ), 0 );
		if ( nevents < 0 ) {
			m_log->error( "poll error" );
			break;
		}

		if ( nevents == 0 ) {
			continue;
		}

		for ( auto it = m_ctx->targets.begin( ); it < m_ctx->targets.end( ); ++it ) {
			if ( on_poll( *it ) ) {
				sock::reset( it->fd );
				m_ctx->targets.erase( it );
			}
		}
	}
}