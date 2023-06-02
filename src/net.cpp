#include <include.h>

#include <net.h>

void net::cli::on_poll( uv_poll_t *handle, int status, int flags ) {
    auto cli = ( cli_context_t * )handle->loop->data;
    if ( !cli ) {
        spdlog::critical( "missing client context, exiting..." );
        uv_poll_stop( handle );
        return;
    }

    auto srv = ( net::cli::server_descriptor_t * )handle->data;

    if ( status < 0 ) {
        cli->log->error( "poll error, {}", uv_strerror( status ) );
        srv->close( );
        return;
    }

    if ( !( flags & UV_READABLE ) ) {
        return;
    }

    // pop a buffer from the buffer pool
    const auto buf = cli->bufpool.get( );
    if ( !buf ) {
        cli->log->debug( "no available buffers, discarding msg" );
        return;
    }

    int ret = srv->read( buf );
    if ( ret <= 0 ) {
        cli->log->error( "failed to read from socket, {}", ret );
        cli->bufpool.release( buf ); // release

        cli->remove_target( srv->sock.fd );

        srv->close( );

        return;
    }

    cli->queue.try_put( msg_t{ buf, ( size_t )ret, srv->sock.fd } );
}

void net::server::on_connect( uv_poll_t *handle, int status, int events ) {
    auto ctx = ( server_context_t * )handle->loop->data;

    if ( status < 0 ) {
        ctx->log->error( "on_connect error, {}", uv_strerror( status ) );
        return;
    }

    if ( !( events & UV_READABLE ) ) {
        return;
    }

    auto session = ctx->alloc_session( );

    int ret = mbedtls_net_accept( &ctx->sock, &session->sock, nullptr, 0, nullptr );
    if ( ret != 0 ) {
        ctx->log->warn( "failed to accept new session, {}", ret );
        ctx->dealloc_session( session );
        return;
    }

    ctx->log->info( "new session: {}", session->sock.fd );

    uv_poll_init_socket( handle->loop, &session->handle, session->sock.fd );

    session->handle.data = session;

    uv_poll_start( &session->handle, UV_READABLE | UV_DISCONNECT, on_read );

    session->fix.state = Idle;
}

void net::server::on_read( uv_poll_t *handle, int status, int events ) {
    auto ctx = ( server_context_t * )handle->loop->data;

    if ( status < 0 ) {
        ctx->log->error( "on_read error, {}", uv_strerror( status ) );
        uv_poll_stop( handle );
        return;
    }

    auto s = ( session_t * )handle->data;

    if ( events & UV_DISCONNECT ) {
        ctx->log->warn( "session disconnected, {}", s->sock.fd );

        s->close( );
        ctx->dealloc_session( s );
        return;
    }

    if ( events & UV_READABLE ) {
        auto buf = ctx->bufpool.get( );
        if ( !buf ) {
            ctx->log->warn( "no available buffers." );
            return;
        }

        int nread = mbedtls_net_recv( &s->sock, ( unsigned char * )buf, ctx->bufpool.buf_size );

        if ( nread <= 0 ) {
            ctx->log->warn( "error reading from session, {}", nread );

            s->close( );
            ctx->dealloc_session( s );
            ctx->bufpool.release( buf );
            return;
        }

        // ctx->log->debug( "received {} bytes from {}, {}", nread, s->sock.fd, str );

        ctx->in_q.try_put( session_message_t{ buf, s, static_cast< size_t >( nread ), message_state::Ok } );
    }
}
