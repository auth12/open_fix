#include "include.h"

#include <net.h>

#include <server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    std::shared_ptr< net::tcp_server_context_t > &ctx;
    on_read_node_t( std::shared_ptr< net::tcp_server_context_t > &c ) : ctx( c ) {}

    void operator( )( net::net_msg_t msg ) {
        const auto buf = msg.cast_to< std::span< char > >( );
        const int fd = msg.tag( );

        if ( ctx->sessions[ fd ]->state( ) < net::Idle ) {
            ctx->bufpool.release( buf.data( ) );
            return;
        }

        ctx->log->debug( "got msg from fd {}", fd );

        auto tp = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::steady_clock::now( ).time_since_epoch( ) );
        ctx->log->debug( "time received: {}", tp.count( ) );

        ctx->bufpool.release( buf.data( ) );
    }
};

int main( ) {
    net::tcp_server srv( "tcp_server", false );
    auto &ctx = srv.ctx( );

    if( srv.bind( HOST , PORT ) != 0 ) {
        ctx->log->error( "failed to bind" );
        return 0;
    }

    ctx->log->info( "listening on {}:{}", HOST, PORT );

    tbb::flow::function_node< net::net_msg_t > on_read{ srv.graph( ), tbb::flow::serial, on_read_node_t( ctx ) };
    tbb::flow::make_edge( srv.queue_node( ), on_read );

    return srv.run( );
};