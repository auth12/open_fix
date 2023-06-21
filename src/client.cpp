#include "include.h"

#include "client.h"

net::tcp_client::tcp_client( const std::string_view log_name, bool to_file )
	: m_ctx{ std::make_shared< tcp_client_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) } {
	m_log->flush_on( spdlog::level::n_levels );
	m_log->set_level( spdlog::level::debug );
	m_log->set_pattern( "[%t]%+" );

	uv_loop_init( &m_loop );
	m_loop.data = this;
}

void net::client_cb::on_poll( uv_poll_t *handle, int status, int flags ) {
	auto cli = ( tcp_client * )handle->loop->data;
	auto session = ( tcp_session * )handle->data;

	auto &log = cli->log( );
	auto &ctx = cli->ctx( );

	if ( flags & UV_READABLE ) {
		auto buf = ctx->bufpool.get( );
		if ( !buf ) {
			log->warn( "no available buffers in pool" );
			return;
		}

		int ret = mbedtls_net_recv( session->net_ctx( ), ( unsigned char * )buf, cli_buf_size );
		if ( ret <= 0 ) {
			log->warn( "recv returned {}, closing...", ret );

			session->close( );
			session->poll_stop( );

			ctx->bufpool.release( buf );
			return;
		}

		auto msg = std::make_unique< message::net_msg_t >( );
		msg->buf = buf;
		msg->len = ret;
		msg->session = session;

		if ( !ctx->msg_queue.try_push( std::move( msg ) ) ) {
			log->critical( "failed to push msg" );
		}
	}
}

int net::tcp_client::connect( const std::string_view host, const std::string_view port ) {
	auto session = std::make_shared< tcp_session >( );

	int ret = mbedtls_net_connect( session->net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
	if ( ret != 0 ) {
		return ret;
	}

	m_log->info( "connected to {}:{}", host, port );

	ret = session->poll_init( &m_loop );
	if ( ret != 0 ) {
		return ret;
	}

	session->poll_handle( )->data = session.get( );
	session->set_state( Idle );

	ret = session->poll_start( client_cb::on_poll, UV_READABLE );
	if ( ret != 0 ) {
		return ret;
	}

	m_log->info( "polling..." );

	if( !m_callbacks[ ON_CONNECT ].empty( ) ) {
		for( auto &cb : m_callbacks[ ON_CONNECT ] ) {
			cb( session.get( ) );
		}
	}

	m_ctx->targets[ session->fd( ) ].swap( session );

	return ret;
}