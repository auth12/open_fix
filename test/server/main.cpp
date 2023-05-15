#include "include.h"

#include <net.h>

#include <any>

struct on_msg_recv {
    std::shared_ptr< net::server::server_context_t > ctx;
    std::any a;

    on_msg_recv( std::shared_ptr< net::server::server_context_t > &_c ) : ctx{ _c } {}

    void operator( )( net::server::session_message_t msg ) {
        std::string_view buf( msg.buf, msg.len );
        ctx->log->info( "ptr: {:x}, size: {}, buf: {}", uintptr_t( msg.buf ), buf.size( ), buf );

        ctx->bufpool.release( msg.buf );
    }
};

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::server::server_context_t * )handle->data;

    if ( ctx->sessions.empty( ) ) {
        return;
    }

    ctx->log->info( "broadcasting..." );
    for ( auto &[ sock, session ] : ctx->sessions ) {
        int ret = session->write( "Hello" );
        ctx->log->debug( "sent {} bytes to {}", ret, sock );
    }
}

int main( ) {
    net::server::tcp_server_t< on_msg_recv > srv( "127.0.0.1", "1515" );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    uv_timer_t timer;
    srv.register_loop_timer( &timer, on_timer_cb, 1 );

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};