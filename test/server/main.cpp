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

int main( ) {
    net::server::tcp_server_t< on_msg_recv > srv( "127.0.0.1", "1515" );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};