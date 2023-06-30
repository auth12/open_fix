#include "include.h"

#include "client.h"

int net::tcp_client::connect( const std::string_view host, const std::string_view port ) {
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

		ret = ::connect( sockfd, cur->ai_addr, cur->ai_addrlen );
		if ( ret < 0 ) {
			close( sockfd );
			sockfd = -1;
			continue;
		}

		break;
	}

	freeaddrinfo( info );

	if ( !sockfd ) {
		return -1;
	}

	ret = fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFL, 0 ) | O_NONBLOCK );
	if ( ret != 0 ) {
		m_log->warn( "[ TCP ] failed to set socket to non block." );
		close( sockfd );
		return -1;
	}

	auto session = std::make_shared< tcp_session >( );
	session->set_fd( sockfd );
	session->poll_init( &m_loop );
	session->poll_handle( )->data = session.get( );
	session->poll_start( cb::on_poll, UV_READABLE | UV_DISCONNECT );

	m_ctx->targets[ sockfd ].swap( session );

	return sockfd;
}

void net::cb::on_poll( uv_poll_t *handle, int status, int flags ) {
	auto cli = ( net::tcp_client * )handle->loop->data;
	auto session = ( net::tcp_session * )handle->data;

	auto log = cli->log( );
	auto ctx = cli->ctx( );
	
	if ( status < 0 ) {
		log->error( "poll error, {}", uv_strerror( status ) );
		return;
	}

	if ( flags & UV_DISCONNECT ) {
		session->reset( );
		session->poll_stop( );
		return;
	}

	if ( flags & UV_READABLE ) {
		char *buf = ctx->bufpool.get( );
		if ( !buf ) {
			log->warn( "no available buffers" );
			return;
		}

		const int fd = session->fd( );

		ssize_t nread = session->read( buf, ctx->bufpool.obj_size( ) );
		if ( nread == EWOULDBLOCK ) {
			ctx->bufpool.release( buf );
			return;
		}

		if ( nread <= 0 ) {
			log->warn( "read returned {} on fd: {}", nread, fd );
			ctx->bufpool.release( buf );
			session->reset( );
			session->poll_stop( );
			return;
		}

		auto msg = std::make_unique< message::net_msg_t >( );
		msg->buf = buf;
		msg->len = nread;
		msg->fd = fd;

		ctx->in_queue.push( std::move( msg ) );
	}
}