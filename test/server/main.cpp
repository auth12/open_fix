#include "include.h"

#include <server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    net::tcp_server &server;
    on_read_node_t( net::tcp_server &p_server ) : server{ p_server } {}

    void operator( )( message::net_msg_t msg ) {
        const auto buf = msg.cast_to< std::span< char > >( );
        const int fd = msg.tag( );

        auto &ctx = server.ctx( );
        auto &log = server.log( );

        if ( ctx->sessions[ fd ]->state( ) < net::Idle ) {
            ctx->bufpool.release( buf.data( ) );
            return;
        }

        log->debug( "got msg from fd {}", fd );

        auto tp = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::steady_clock::now( ).time_since_epoch( ) );
        log->debug( "time received: {}", tp.count( ) );

        ctx->bufpool.release( buf.data( ) );
    }
};

int main( ) {
    net::tcp_server srv( "tcp_server", false );
    auto &ctx = srv.ctx( );
    auto &log = srv.log( );

    if ( srv.bind( HOST, PORT ) != 0 ) {
        log->error( "failed to bind" );
        return 0;
    }

    log->info( "listening on {}:{}", HOST, PORT );

    tbb::flow::function_node< message::net_msg_t > on_read{ srv.graph( ), tbb::flow::unlimited, on_read_node_t( srv ) };
    tbb::flow::make_edge( srv.queue_node( ), on_read );

    return srv.run( );
};