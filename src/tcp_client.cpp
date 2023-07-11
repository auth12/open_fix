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
		return getaddrinfo_err;
	}

	struct addrinfo *cur;
	for ( cur = info; cur != nullptr; cur = info->ai_next ) {
		sockfd = socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
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
		return connect_err;
	}

	m_ctx->log->debug( "Connected to {}:{}, setting socket to nonblock...", host, port );

	ret = fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) | O_NONBLOCK );
	if ( ret != 0 ) {
		m_ctx->log->error( "Failed to set socket to non block." );
		close( sockfd );
		return sock_err;
	}

	m_ctx->log->debug( "Set socket {} to non block.", sockfd );

	auto session = &( m_ctx->targets.emplace_back( tcp_session{ &m_loop, sockfd } ) );
	session->poll_handle( )->data = session;
	if ( session->poll_start( cb::on_poll, UV_READABLE | UV_DISCONNECT ) == 0 ) {
		m_ctx->log->debug( "Polling session fd: {}, target: {}:{}", sockfd, host, port );
	}

	return sockfd;
}

void net::cb::on_poll( uv_poll_t *handle, int status, int flags ) {
	auto ctx = ( net::tcp_client_ctx_t * )handle->loop->data;
	auto session = ( net::tcp_session * )handle->data;

	const int fd = session->fd( );

	if ( status < 0 ) {
		ctx->log->error( "Poll error, {}", uv_strerror( status ) );
		return;
	}

	if ( flags & UV_DISCONNECT ) {
		ctx->log->warn( "Connection reset by peer." );

		session->reset( );
		session->poll_stop( );

		ctx->erase_target( fd );
		return;
	}

	if ( flags & UV_READABLE ) {
		char *buf = ctx->bufpool.get( ); // busy waits
		if ( !buf ) {
			ctx->log->warn( "No available buffers in pool." );
			return;
		}

		ssize_t nread = session->read( buf, ctx->bufpool.obj_size );
		if ( nread == EWOULDBLOCK ) {
			ctx->bufpool.release( buf );
			return;
		}

		ctx->log->debug( "IN:{} -> Read {} bytes.", fd, nread );

		if ( nread <= 0 ) {
			ctx->log->warn( "Read returned {} on fd: {}", nread, fd );

			ctx->bufpool.release( buf );
			session->reset( );
			session->poll_stop( );

			ctx->erase_target( fd );
			return;
		}

		ctx->in_queue.push( message::net_msg_t{ fd, buf, ( size_t )nread } );
	}
}