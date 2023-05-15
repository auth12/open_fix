#include "include.h"

#include <net.h>

#include <fix_spec.h>

struct on_msg_node_t {
    std::shared_ptr< net::client_context_t > ctx;
    on_msg_node_t( std::shared_ptr< net::client_context_t > &_c ) : ctx{ _c } {}

    void operator( )( net::msg_t msg ) {
        std::string_view buf( msg.buf.ptr, msg.buf.len );
        ctx->log->debug( "ptr: {:x}, size: {}, buf: {}", uintptr_t( msg.buf.ptr ), buf.size( ), buf );

        ctx->bufpool.release( msg.buf.ptr );
    }
};


int main( ) {
    net::tcp_client_t< on_msg_node_t > cli( "127.0.0.1", "1515" );
    auto &ctx = cli.ctx;

    // init dictionaries
    std::map< int, std::string > field_dic;
    hffix::dictionary_init_field( field_dic );

    int seq = 0;

    char buffer[ 4096 ];
    hffix::message_writer logon( buffer, buffer + sizeof( buffer ) );
    logon.push_back_header( "FIX.4.4" );

    auto tp = std::chrono::high_resolution_clock( ).now( );

    logon.push_back_string( hffix::tag::MsgType, fix_spec::MsgType_val::LOGON );
    logon.push_back_string( hffix::tag::SenderCompID, "ASHIMKTUAT001" );
    logon.push_back_string( hffix::tag::TargetCompID, "GEMINIMKTUAT" );
    //logon.push_back_int( hffix::tag::MsgSeqNum, seq++ );
    logon.push_back_timestamp( hffix::tag::SendingTime, tp );
    // No encryption.
    logon.push_back_int( hffix::tag::EncryptMethod, 0 );
    
    // 10 second heartbeat interval.
    logon.push_back_int( hffix::tag::HeartBtInt, 30 );

    logon.push_back_trailer( ); // write CheckSum.

    ctx->log->info( "{}", buffer );

    int ret = cli.connect( );
    if ( ret != 0 ) {
        ctx->log->error( "failed to connect to host." );

        return 0;
    }

    cli.ctx->write( buffer, logon.message_size( ) );

    return cli.run( );
}