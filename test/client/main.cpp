#include "include.h"

#include <net.h>

#include <fix_parse.h>

#include <config.h>

struct on_msg_node_t {
    std::shared_ptr< net::cli::cli_context_t > ctx;
    on_msg_node_t( std::shared_ptr< net::cli::cli_context_t > &_c ) : ctx{ _c } {}

    void operator( )( net::cli::msg_t msg ) {
        std::string_view buf{ msg.buf, msg.len };
        ctx->log->debug( "ptr: {:x}, size: {}", uintptr_t( msg.buf ), buf.size( ) );
        ctx->log->info( buf );

        ctx->bufpool.release( msg.buf );
    }
};

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::cli::cli_context_t * )handle->data;

    if( ctx->targets.empty( ) ) {
        uv_timer_stop( handle );
        return;
    }

    static char buf[ 2048 ];

    for ( auto &[ sock, srv ] : ctx->targets ) {
        fix::fix_writer_t wr{ buf, sizeof( buf ) };
        wr.push_header( fmt::format( "FIX.{}.{}", srv->FIX_major, srv->FIX_minor ) );
        wr.push_int( fix_spec::MsgSeqNum, srv->fix.next_out++ );
        
        wr.push_trailer( );

        srv->write( buf, wr.cur_pos );

        auto tp = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::steady_clock::now( ).time_since_epoch( ) );
        ctx->log->info( "time sent: {}", tp.count( ) );

        memset( buf, 0, sizeof( buf ) );
    }
}

int main( int argc, char **argv ) {
    config::cli_fix_cfg_t cfg;

    if ( !config::get_cli_config( "fix.json", cfg ) ) {
        spdlog::error( "failed to parse client config file" );
        return 0;
    }

    net::cli::tcp_cli_t< on_msg_node_t > cli;
    auto &ctx = cli.ctx;

    for ( auto &s : cfg.targets ) {
        const auto cur = s.ip.find( ':' );
        if ( cur == std::string::npos ) {
            spdlog::warn( "invalid address specified in config file" );
            continue;
        }
        auto ip = s.ip.substr( 0, cur );
        auto port = s.ip.substr( cur + 1 );

        int ret = cli.connect( ip, port );
        if ( ret != 0 ) {
            ctx->log->error( "failed to connect to {}:{}.", ip, port );
            continue;
        }
    };

    if ( ctx->targets.empty( ) ) {
        return 0;
    }

    uv_timer_t t;
    t.data = ctx.get( );
    uv_timer_init( &cli.loop, &t );
    uv_timer_start( &t, on_timer_cb, 0, 10 );

    return cli.run( );
}