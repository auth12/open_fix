#include "server.h"

net::tcp_server::tcp_server( const std::string_view log_name, const bool to_file )
    : m_ctx{ std::make_shared< tcp_server_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) },
      m_message_queue{ m_message_graph } {
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

    return m_ctx->server_session.poll_start( on_server_poll, UV_READABLE );
}

void net::on_server_poll( uv_poll_t *handle, int status, int events ) {
    auto *srv = ( tcp_server * )handle->loop->data;
    auto &log = srv->log( );
    auto &ctx = srv->ctx( );

    if ( status < 0 ) {
        log->error( "server poll error, {}", uv_strerror( status ) );
        return;
    }

    if ( !( events & UV_READABLE ) )
        return;

    auto session = std::make_shared< net::tcp_session >( );

    uint8_t ip[ 4 ];
    size_t ip_len = 0;
    int ret = mbedtls_net_accept( ctx->server_session.net_ctx( ), session->net_ctx( ), ip, sizeof( ip ), &ip_len );
    if ( ret != 0 ) {
        log->error( "failed to accept new session, {}", ret );
        return;
    }

    // ipv4 only
    session->set_ip( fmt::format( "{}.{}.{}.{}", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] ) );

    log->info( "new session, fd: {}, ip: {}", session->fd( ), session->ip( ) );

    if ( session->poll_init( handle->loop ) != 0 ) {
        log->error( "failed to initialize session poll handle." );
        session->close( );
        return;
    }

    session->poll_handle( )->data = session.get( );

    if ( session->poll_start( on_session_poll, UV_READABLE | UV_DISCONNECT ) != 0 ) {
        log->error( "failed to initialize session poll handle." );
        session->close( );
        return;
    }

    session->set_state( Idle );

    ctx->sessions[ session->fd( ) ].swap( session );
}

void net::on_session_poll( uv_poll_t *handle, int status, int events ) {
    auto srv = ( tcp_server * )handle->loop->data;
    auto &ctx = srv->ctx( );
    auto &log = srv->log( );

    auto session = ( tcp_session * )handle->data;

    if ( status < 0 ) {
        log->error( "session poll error, {}", uv_strerror( status ) );

        session->close( );
        session->poll_stop( );
        return;
    }

    if ( events & UV_DISCONNECT ) {
        log->warn( "session fd: {}, ip: {} disconnected", session->fd( ), session->ip( ) );
        session->close( );
        session->poll_stop( );
        return;
    }

    if ( !( events & UV_READABLE ) ) {
        return;
    }

    auto buf = ctx->bufpool.get( );
    if ( !buf ) {
        log->warn( "no available buffer in the buffer pool, waiting..." );
        return;
    }

    int nread = session->read( buf, ctx->bufpool.buf_size );
    if ( nread <= 0 ) {
        log->error( "read from session returned {}, dropping", nread );

        session->close( );
        session->poll_stop( );

        ctx->bufpool.release( buf );
        return;
    }

    log->debug( "read {} bytes from {}, fd: {}", nread, session->ip( ), session->fd( ) );
    srv->queue_node( ).try_put( { session->fd( ), std::span{ buf, ( size_t )nread } } );
}