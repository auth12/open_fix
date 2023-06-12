#include "server.h"

net::tcp_server::tcp_server( const std::string_view log_name, const bool to_file,
							 const unsigned int msg_queue_elements )
	: m_ctx{ std::make_shared< tcp_server_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) },
	  m_queue{ msg_queue_elements } {
	m_log->flush_on( spdlog::level::n_levels );
	m_log->set_level( spdlog::level::debug );
	m_log->set_pattern( "[%t]%+" );

	uv_loop_init( &m_loop );
	m_loop.data = this;

	m_ctx->server_session.init_handle( &m_loop );

	uv_sem_init( &m_sem, 0 ); // add back server busy wait later
}

int net::tcp_server::bind( const std::string_view host, const uint16_t port ) {
	struct sockaddr_in dest;

	int ret = uv_ip4_addr( host.data( ), port, &dest );
	if ( ret != 0 ) {
		return ret;
	}

	ret = uv_tcp_bind( m_ctx->server_session.tcp_handle( ), ( struct sockaddr * )&dest, 0 );
	if ( ret != 0 ) {
		return ret;
	}

	return uv_listen( ( uv_stream_t * )m_ctx->server_session.tcp_handle( ), 0, server_cb::on_session_connect );
}

void net::server_cb::on_buf_alloc( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf ) {
	auto cli = ( tcp_server * )handle->loop->data;
	auto &ctx = cli->ctx( );

	buf->base = ctx->bufpool.get( );
	buf->len = ctx->bufpool.obj_size( );
}

void net::server_cb::on_session_connect( uv_stream_t *server_handle, int status ) {
	auto server = ( tcp_server * )server_handle->loop->data;
	auto &log = server->log( );
	auto &ctx = server->ctx( );

	if ( status < 0 ) {
		log->warn( "on connect error, {}", uv_strerror( status ) );
		return;
	}

	auto session = std::make_shared< tcp_session >( );

	session->init_handle( server_handle->loop );

	int ret = uv_accept( server_handle, ( uv_stream_t * )session->tcp_handle( ) );
	if ( ret != 0 ) {
		log->error( "failed to accept new session, {}", uv_strerror( ret ) );

		session->close( );
		return;
	}

	int fd = -1;
	ret = uv_fileno( ( uv_handle_t * )session->tcp_handle( ), &fd );
	if ( ret != 0 ) {
		log->error( "failed to grab session file descriptor {}", fd );

		session->close( );
		return;
	}

	log->info( "new session connected, fd {}", fd );

	session->tcp_handle( )->data = session.get( );
	session->set_fd( fd );

	ret = uv_read_start( ( uv_stream_t * )session->tcp_handle( ), server_cb::on_buf_alloc, server_cb::on_read );
	if ( ret != 0 ) {
		log->error( "failed to start reading, {}", uv_strerror( ret ) );

		session->close( );
	}
	
	session->set_state( Idle );

	ctx->sessions[ fd ].swap( session );
}

void net::server_cb::on_read( uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf ) {
	auto srv = ( tcp_server * )handle->loop->data;
	auto &ctx = srv->ctx( );
	auto &log = srv->log( );

	auto session = ( tcp_session * )handle->data;

	if ( nread <= 0 ) {
		log->error( "read from session returned {}, dropping", uv_strerror( nread ) );

		session->close( );
		ctx->bufpool.release( buf->base );
		return;
	}

	log->debug( "read {} bytes, fd: {}", nread, session->fd( ) );

	auto msg = std::make_unique< message::net_msg_t >( );
	msg->buf = buf->base;
	msg->len = nread;
	msg->session = session;

	srv->msg_queue( ).push( std::move( msg ) );
	uv_sem_post( srv->get_sem( ) );
}