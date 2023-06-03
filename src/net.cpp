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
        cli->bufpool.release( buf );

        srv->close( );

        return;
    }

    cli->queue.try_put( { srv->sock.fd, std::span{ buf, ( size_t )ret } } );
}