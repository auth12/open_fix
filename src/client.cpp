#include "include.h"

#include "client.h"

net::tcp_client::tcp_client( const std::string_view log_name, bool to_file, const unsigned int msg_queue_elements )
	: m_ctx{ std::make_shared< tcp_client_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) } {
	m_log->flush_on( spdlog::level::n_levels );
	m_log->set_level( spdlog::level::debug );
	m_log->set_pattern( "[%t]%+" );

	uv_loop_init( &m_loop );
	m_loop.data = this;
}

void net::client_cb::on_buf_alloc( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf ) {
	auto cli = ( tcp_client * )handle->loop->data;
	auto &ctx = cli->ctx( );
	buf->base = ctx->bufpool.get( );
	buf->len = ctx->bufpool.obj_size( );
}

void net::client_cb::on_read( uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf ) {
	auto cli = ( tcp_client * )handle->loop->data;
	auto session = ( tcp_session * )handle->data;

	auto &log = cli->log( );
	auto &ctx = cli->ctx( );

	if ( nread <= 0 ) {
		log->warn( "failed to read from socket, {}", uv_strerror( nread ) );

		session->close( );

		ctx->bufpool.release( buf->base );
		return;
	}

	log->info( "got msg {}", std::string_view{ buf->base, ( size_t )nread } );

	auto msg = std::make_unique< message::net_msg_t >( );
	msg->buf = buf->base;
	msg->len = nread;
	msg->session = session;

	cli->msg_queue( ).push( std::move( msg ) );
}

void net::client_cb::on_connect( uv_connect_t *req, int status ) {
	auto session = ( tcp_session * )req->handle->data;
	auto cli = ( tcp_client * )req->handle->loop->data;

	auto &ctx = cli->ctx( );
	auto &log = cli->log( );

	if ( status < 0 ) {
		log->error( "failed to connect, {}", uv_strerror( status ) );
		return;
	}

	int fd = -1;
	int ret = uv_fileno( ( uv_handle_t * )req->handle, &fd );
	if ( ret != 0 ) {
		log->error( "failed to grab session file descriptor {}", fd );

		session->close( );
		return;
	}

	session->set_fd( fd );
	session->set_state( Idle );

	log->info( "connected." );

	if ( cli->con_fn( ) ) {
		cli->con_fn( )( session );
	}

	ret = uv_read_start( ( uv_stream_t * )req->handle, on_buf_alloc, on_read );
	if ( ret != 0 ) {
		log->error( "failed to start reading {}", uv_strerror( ret ) );

		session->close( );
	}
}

int net::tcp_client::connect( const std::string_view host, const uint16_t port ) {
	auto session = *( m_ctx->targets.emplace_back( std::make_shared< tcp_session >( ) ) );

	struct sockaddr_in dest;
	int ret = uv_ip4_addr( host.data( ), port, &dest );
	if ( ret != 0 ) {
		return ret;
	}

	session->init_handle( &m_loop );
	session->tcp_handle( )->data = session.get( );

	return uv_tcp_connect( session->connect_request( ), session->tcp_handle( ), ( struct sockaddr * )&dest,
						   client_cb::on_connect );
}