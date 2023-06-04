#include "include.h"

#include "client.h"

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