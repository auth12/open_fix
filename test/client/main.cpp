#include "include.h"

#include <net.h>

#include <fix_parse.h>

#include <config.h>

struct on_msg_node_t {
    std::shared_ptr< net::cli::cli_context_t > ctx;
    on_msg_node_t( std::shared_ptr< net::cli::cli_context_t > &_c ) : ctx{ _c } {}

    void operator( )( net::cli::msg_t msg ) {
        std::string_view buf{ msg.buf, msg.len };

        ctx->log->info( "{}", buf );

        if ( fix::is_valid_fix( buf ) ) {
            fix::fix_message_t m{ buf };
            /*auto it = m.find( fix_spec::tag::MDEntryPx );
            if ( it != m.end( ) ) {
                ctx->log->info( "got px update: {}", it->val.as_float( ) );
            }*/
        }

        ctx->bufpool.release( msg.buf );
    }
};

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=A\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::cli::cli_context_t * )handle->data;
    ctx->write( fix_buf, sizeof( fix_buf ) );
}

int main( ) {
    config::cli_fix_cfg_t cfg;

    if( !config::get_cli_config( "fix.config", cfg ) ) {
        spdlog::error( "failed to parse client config file" );
        return 0;
    }

    spdlog::info( "{},{},{},{}", cfg.ip, cfg.port, cfg.sender_id, cfg.target_id );

    net::cli::tcp_cli_t< on_msg_node_t > cli( cfg.ip, cfg.port );
    auto &ctx = cli.ctx;

    int ret = cli.connect( );
    if ( ret != 0 ) {
        ctx->log->error( "failed to connect to host." );

        return 0;
    }

    uv_timer_t t;
    t.data = ctx.get( );
    uv_timer_init( &cli.loop, &t );
    uv_timer_start( &t, on_timer_cb, 0, 1 );

    /*int seq = 7;
    char buf[ 1024 ];
    hffix::message_writer logon( buf, buf + sizeof( buf ) );

    logon.push_back_header( "FIX.4.4" ); // Write BeginString and BodyLength.

    logon.push_back_string( hffix::tag::MsgType, "A" );
    logon.push_back_string( hffix::tag::SenderCompID, "ASHIMKTUAT001" );
    logon.push_back_string( hffix::tag::TargetCompID, "GEMINIMKTUAT" );
    logon.push_back_int( hffix::tag::MsgSeqNum, seq++ );
    logon.push_back_timestamp( hffix::tag::SendingTime, std::chrono::system_clock::now( ) );
    // No encryption.
    logon.push_back_int( hffix::tag::EncryptMethod, 0 );
    // 10 second heartbeat interval.
    logon.push_back_int( hffix::tag::HeartBtInt, 30 );

    logon.push_back_trailer( ); // write CheckSum.

    cli.ctx->log->info( "{}", buf );

    cli.ctx->write( buf, logon.message_size( ) );*/

    return cli.run( );
}