#include "include.h"

#include "client.h"

net::tcp_client::tcp_client( const std::string_view log_name, bool to_file )
    : m_ctx{ std::make_shared< tcp_client_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) },
      m_message_queue{ m_message_graph } {
    m_log->flush_on( spdlog::level::n_levels );
    m_log->set_level( spdlog::level::debug );
    m_log->set_pattern( "[%t]%+" );

    uv_loop_init( &m_loop );
    m_loop.data = this;
}

int net::tcp_client::connect( const std::string_view host, const std::string_view port ) {
    auto session = std::make_shared< tcp_session >( );

    int ret = mbedtls_net_connect( session->net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
    if ( ret != 0 ) {
        return ret;
    }

    ret = session->poll_init( &m_loop );
    if ( ret != 0 ) {
        session->close( );
        return ret;
    }

    session->poll_handle( )->data = session.get( );

    session->set_state( Idle );

    ret = session->poll_start( on_target_poll, UV_READABLE );
    if ( ret != 0 ) {
        session->close( );
        return ret;
    }

    m_ctx->targets[ session->fd( ) ].swap( session );

    return ret;
}

void net::on_target_poll( uv_poll_t *handle, int status, int flags ) {
    auto cli = ( tcp_client * )handle->loop->data;
    auto ctx = cli->ctx( );
    auto log = cli->log( );

    auto session = ( net::tcp_session * )handle->data;

    if ( status < 0 ) {
        log->error( "poll error, {}", uv_strerror( status ) );

        session->close( );
        session->poll_stop( );
        return;
    }

    if ( !( flags & UV_READABLE ) ) {
        return;
    }

    const auto buf = ctx->bufpool.get( );
    if ( !buf ) {
        log->warn( "no available buffers" );
        return;
    }

    int ret = session->read( buf, ctx->bufpool.buf_size );
    if ( ret <= 0 ) {
        log->error( "failed to read from socket, {}", ret );
        ctx->bufpool.release( buf );

        ctx->targets.unsafe_erase( session->fd( ) );

        session->close( );
        session->poll_stop( );

        return;
    }

    cli->queue_node( ).try_put( { session->fd( ), std::span{ buf, ( size_t )ret } } );
}