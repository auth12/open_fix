#include "include.h"

#include <net.h>

#include <fix_parse.h>

std::chrono::time_point< std::chrono::high_resolution_clock > send_time;
std::chrono::time_point< std::chrono::high_resolution_clock > receive_time;

struct on_recv_server_t {
    std::shared_ptr< net::server::server_context_t > ctx;

    on_recv_server_t( std::shared_ptr< net::server::server_context_t > &_c ) : ctx{ _c } {}

    void operator( )( const net::server::session_message_t &msg ) {
        std::string_view buf( msg.buf, msg.len );
        // ctx->log->info( "ptr: {:x}, size: {}, buf: {}", uintptr_t( msg.buf ), buf.size( ), buf );

        // only expect fix
        // hffix::message_reader f_msg{ msg.buf, msg.len };

        ctx->bufpool.release( msg.buf );
    }
};

struct on_recv_client_t {
    std::shared_ptr< net::client_context_t > ctx;
    on_recv_client_t( std::shared_ptr< net::client_context_t > &_c ) : ctx{ _c } {}

    void operator( )( const net::msg_t &msg ) {
        receive_time = std::chrono::high_resolution_clock::now( );
        std::string_view buf( msg.buf.ptr, msg.buf.len );
        // ctx->log->debug( "{}", uintptr_t( msg.buf.ptr ), buf.size( ), buf );

        hffix::message_reader rd{ msg.buf.ptr, msg.buf.len };
        for ( auto &f : rd ) {
        }

        hffix::message_reader_const_iterator it = rd.end( );
        if( rd.find_with_hint( fix_spec::tag::MDEntryPx, it ) ) {
            ctx->log->info( "got px update: {}", it->value( ).as_string( ) );
        }

        /*if ( fix::is_valid_fix( buf ) ) {
            fix::fix_message_t m{ buf };
            auto it = m.find( fix_spec::tag::MDEntryPx );
            if ( it != m.end( ) ) {
                ctx->log->info( "got px update: {}", it->val.as_float( ) );
            }
        }*/

        ctx->bufpool.release( msg.buf.ptr );
    }
};

constexpr char fix_buf[] = "8=FIX.4.49=17835=W49=SENDER56=RECEIVER34=12352=20230517-09:30:00."
                           "00055=EUR/USD262=1268=2269=0270=1.2345271=10000010=080";

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::server::server_context_t * )handle->data;
    if ( ctx->sessions.empty( ) ) {
        return;
    }

    // ctx->log->info( "broadcasting..." );
    for ( auto &[ sock, session ] : ctx->sessions ) {
        int ret = session->write( fix_buf );
        // send_time = std::chrono::high_resolution_clock::now( );

        // ctx->log->debug( "sent {} bytes to {}", ret, sock );
    }
}

#define HOST "127.0.0.1"
#define PORT "1515"

int main( ) {
    net::server::tcp_server_t< on_recv_server_t > srv( HOST, PORT );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    srv.ctx->log->info( "serving on {}:{}", HOST, PORT );

    uv_timer_t timer;
    srv.register_loop_timer( &timer, on_timer_cb, 10 );

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};

/*
int main( ) {
    std::thread( []( ) {
        net::server::tcp_server_t< on_recv_server_t > srv( HOST, PORT );

        if ( srv.bind( ) != 0 ) {
            return;
        }

        uv_timer_t timer;
        srv.register_loop_timer( &timer, on_timer_cb, 10 );

        srv.run( );
    } ).detach( );

    std::thread( []( ) {
        net::tcp_client_t< on_recv_client_t > cli( HOST, PORT );
        auto &ctx = cli.ctx;

        int ret = cli.connect( );
        if ( ret != 0 ) {
            ctx->log->error( "failed to connect to host." );

            return 0;
        }

        return cli.run( );
    } ).detach( );

    while ( 1 ) {
        spdlog::info( "roundtrip latency :{}",
                      std::chrono::duration_cast< std::chrono::microseconds >( receive_time - send_time ).count( ) );
        uv_sleep( 1000 );
    }

    return 0;
}*/