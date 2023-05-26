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

        if ( fix::is_valid_fix( buf ) ) {
            fix::fix_message_t m{ buf };
            auto it = m.find( fix_spec::tag::MDEntryPx );
            if ( it != m.end( ) ) {
                ctx->log->info( "got px update: {} from {}", it->val.as_float( ), msg.sock );
            }
        }

        ctx->bufpool.release( msg.buf );
    }
};

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=A\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::cli::cli_context_t * )handle->data;
    for ( auto &[ sock, srv ] : ctx->targets ) {
        srv->write( fix_buf, sizeof( fix_buf ) );
    }
}

int main( ) {
    config::cli_fix_cfg_t cfg;

    if ( !config::get_cli_config( "fix.config", cfg ) ) {
        spdlog::error( "failed to parse client config file" );
        return 0;
    }

    net::cli::tcp_cli_t< on_msg_node_t > cli;
    auto &ctx = cli.ctx;

    for ( auto &s : cfg.targets ) {
        const auto cur = s.find( ':' );
        if ( cur == std::string::npos ) {
            spdlog::warn( "invalid address specified in config file" );
            continue;
        }
        auto ip = s.substr( 0, cur );
        auto port = s.substr( cur + 1 );

        int ret = cli.connect( ip, port );
        if ( ret != 0 ) {
            ctx->log->error( "failed to connect to {}:{}.", ip, port );
            continue;
        }
    };

    if ( ctx->targets.empty( ) ) {
        return 0;
    }

    char nonce[ 32 ];

    for ( int i = 0; i < sizeof( nonce ); i++ ) {
        nonce[ i ] = rand( ) % 256;
    }

    const auto nonce_64 = details::base64_encode( ( unsigned char * )nonce, sizeof( nonce ) );

    auto timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now( ).time_since_epoch( ) )
            .count( );

    auto raw_data = fmt::format( "{}.{}", timestamp, nonce_64 );

    spdlog::info( "{}", raw_data );

    char buf[ 2048 ];
    fix::fix_writer_t wr( buf, sizeof( buf ) );
    wr.push_header( "FIX.4.4" );
    wr.push_field( fix_spec::tag::MsgType, "A" );
    wr.push_field( fix_spec::tag::SenderCompID, "ASHIMKTUAT001" );
    wr.push_field( fix_spec::tag::TargetCompID, "GEMINIKTUAT" );
    wr.push_int( fix_spec::tag::MsgSeqNum, 1 );
    // wr.push_field( fix_spec::tag::Username, "FnVfw2NS" );
    //wr.push_field( fix_spec::tag::Password, "42fSsqVo1ZDgL5PWXSvcCTZH7D0pCqxpML-mKGSarGY" );
    // wr.push_field( fix_spec::tag::RawData, raw_data );
    wr.push_trailer( );

    spdlog::info( buf );

    ctx->targets.begin( )->second->write( buf, wr.cur_pos ); 

    /*uv_timer_t t;
    t.data = ctx.get( );
    uv_timer_init( &cli.loop, &t );
    uv_timer_start( &t, on_timer_cb, 0, 10 );*/

    return cli.run( );
}