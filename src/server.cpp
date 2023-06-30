#include "server.h"

net::tcp_server::tcp_server( const std::string_view log_name, const bool to_file )
	: m_ctx{ std::make_shared< tcp_server_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) } {
	m_log->flush_on( spdlog::level::n_levels );
	m_log->set_level( spdlog::level::debug );
	m_log->set_pattern( "[%t]%+" );

	uv_loop_init( &m_loop );
	m_loop.data = this;
}

int net::tcp_server::bind( const std::string_view host, const std::string_view port ) {
	addrinfo hints, *res;

	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo( host.data( ), port.data( ), &hints, &res );
	if ( ret != 0 ) {
		m_log->error( "failed to get address info, ret {}", ret );
		return ret;
	}

	auto &session = m_ctx->server_session;

	int fd = ::socket( res->ai_family, res->ai_socktype, res->ai_protocol );
	if ( fd < 0 ) {
		m_log->error( "failed to create socket" );

		freeaddrinfo( res );
		return -1;
	}

	ret = ::bind( fd, res->ai_addr, res->ai_addrlen );
	if ( ret != 0 ) {
		m_log->error( "failed to bind." );
		session.reset( );

		freeaddrinfo( res );
		return ret;
	}

	freeaddrinfo( res );

	ret = ::listen( fd, 128 );
	if ( ret != 0 ) {
		m_log->error( "failed to listen" );
		session.reset( );
		return ret;
	}

	session.set_fd( fd );

	ret = m_ctx->server_session.poll_init( &m_loop );
	if ( ret != 0 ) {
		return ret;
	}

	return m_ctx->server_session.poll_start( server_cb::on_connect, UV_READABLE );
}

void net::server_cb::on_connect( uv_poll_t *server_handle, int status, int events ) {
	auto server = ( tcp_server * )server_handle->loop->data;
	auto &log = server->log( );
	auto &ctx = server->ctx( );

	if ( status < 0 ) {
		log->warn( "on connect error, {}", uv_strerror( status ) );
		return;
	}

	auto session = std::make_shared< tcp_session >( );

	sockaddr_storage in_addr;
	socklen_t addr_len = sizeof( sockaddr_storage );
	int fd = accept( ctx->server_session.fd( ), ( sockaddr * )&in_addr, &addr_len );
	if ( fd == -1 ) {
		log->error( "failed to accept new session" );
		return;
	}

	log->info( "new session, fd: {}", fd );

	session->set_fd( fd );

	if ( session->poll_init( server_handle->loop ) != 0 ) {
		log->error( "failed to initialize session poll handle." );
		session->reset( );
		return;
	}

	session->poll_handle( )->data = session.get( );

	if ( session->poll_start( on_read, UV_READABLE | UV_DISCONNECT ) != 0 ) {
		log->error( "failed to initialize session poll handle." );
		session->reset( );
		return;
	}

	ctx->sessions[ session->fd( ) ].swap( session );
}

void net::server_cb::on_read( uv_poll_t *handle, int status, int events ) {
	auto srv = ( tcp_server * )handle->loop->data;
	auto &ctx = srv->ctx( );
	auto &log = srv->log( );

	auto session = ( tcp_session * )handle->data;

	if ( events & UV_DISCONNECT ) {
		log->warn( "session {} disconnected", session->fd( ) );
		session->reset( );
		session->poll_stop( );
		return;
	}

	if ( events & UV_READABLE ) {
		auto buf = ctx->bufpool.get( );
		if ( !buf ) {
			log->warn( "no available buffers in pool" );
			return;
		}

		int ret = recv( session->fd( ), buf, server_buf_size, 0 );
		if ( ret <= 0 ) {
			log->warn( "failed to read from socket, {}", ret );

			session->reset( );
			session->poll_stop( );

			ctx->bufpool.release( buf );
			return;
		}

		log->debug( "read {} bytes from fd {}", ret, session->fd( ) );

		auto msg = std::make_unique< message::net_msg_t >( );
		msg->buf = buf;
		msg->len = ret;
		msg->fd = session->fd( );

		if ( !ctx->msg_queue.try_push( std::move( msg ) ) ) {
			log->critical( "failed to push msg" );
		}
	}
}