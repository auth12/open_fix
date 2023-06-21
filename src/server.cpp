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
	int ret = mbedtls_net_bind( m_ctx->server_session.net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
	if ( ret != 0 ) {
		return ret;
	}

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

	uint8_t ip[ 4 ];
	size_t ip_len = 0;
	int ret = mbedtls_net_accept( ctx->server_session.net_ctx( ), session->net_ctx( ), ip, sizeof( ip ), &ip_len );
	if ( ret != 0 ) {
		log->error( "failed to accept new session, {}", ret );
		return;
	}

	// ipv4 only
	auto ip_str = fmt::format( "{}.{}.{}.{}", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );

	log->info( "new session, fd: {}, ip: {}", session->fd( ), ip_str );

	if ( session->poll_init( server_handle->loop ) != 0 ) {
		log->error( "failed to initialize session poll handle." );
		session->close( );
		return;
	}

	session->poll_handle( )->data = session.get( );

	if ( session->poll_start( on_read, UV_READABLE | UV_DISCONNECT ) != 0 ) {
		log->error( "failed to initialize session poll handle." );
		session->close( );
		return;
	}

	session->set_state( Idle );

	ctx->sessions[ session->fd( ) ].swap( session );
}

void net::server_cb::on_read( uv_poll_t *handle, int status, int events ) {
	auto srv = ( tcp_server * )handle->loop->data;
	auto &ctx = srv->ctx( );
	auto &log = srv->log( );

	auto session = ( tcp_session * )handle->data;

	if ( events & UV_DISCONNECT ) {
		log->warn( "session disconnected" );
		session->close( );
		session->poll_stop( );
		return;
	}

	if ( events & UV_READABLE ) {
		auto buf = ctx->bufpool.get( );
		if ( !buf ) {
			log->warn( "no available buffers in pool" );
			return;
		}

		int ret = mbedtls_net_recv( session->net_ctx( ), ( unsigned char * )buf, server_buf_size );
		if ( ret <= 0 ) {
			log->warn( "failed to read from socket, {}", ret );

			session->close( );
			session->poll_stop( );

			ctx->bufpool.release( buf );
			return;
		}

		log->debug( "read {} bytes from fd {}", ret, session->fd( ) );

		auto msg = std::make_unique< message::net_msg_t >( );
		msg->buf = buf;
		msg->len = ret;
		msg->session = session;

		if ( !ctx->msg_queue.try_push( std::move( msg ) ) ) {
			log->critical( "failed to push msg" );
		}
	}
}