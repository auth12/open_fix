#include "include.h"

#include <server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    net::tcp_server &server;
    on_read_node_t( net::tcp_server &p_server ) : server{ p_server } {}

    void operator( )( message::net_msg_t msg ) {
        const auto buf = msg.cast_to< message::message_buf_t >( );
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

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

void on_timer_cb( uv_timer_t *timer ) {
    auto srv = ( net::tcp_server * )timer->loop->data;

    auto &ctx = srv->ctx( );
    for( auto &[ fd, session ] : ctx->sessions ) {
        if( session->state( ) < net::session_state::Idle )
            continue;

        session->write( fix_buf );
    }
}

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

    uv_timer_t timer;
    uv_timer_init( srv.loop( ), &timer );
    uv_timer_start( &timer, on_timer_cb, 0, 1 );

    return srv.run( );
};