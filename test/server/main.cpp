#include "include.h"

#include <net.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    net::server::ctx_ptr ctx;
    on_read_node_t( net::server::ctx_ptr &c ) : ctx( c ) {}

    net::server::session_message_t operator( )( net::server::session_message_t msg ) {
        if( !msg.session ) {
            ctx->log->critical( "invalid session pointer" );

            return msg;
        }

        std::string_view buf( msg.buf, msg.len );

        auto tp = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::steady_clock::now( ).time_since_epoch( ) );
        ctx->log->info( "time received: {}", tp.count( ) );

        ctx->log->debug( "ptr: {:x}, size: {}", uintptr_t( msg.buf ), buf.size( ) );

        // pre parser validation
        if ( !fix::is_valid_fix( buf ) ) {
            ctx->log->warn( "invalid FIX message, dropping" );
            msg.state = net::server::InvalidFix;
            return msg;
        }

        const fix::fix_message_t rd{ buf };

        // validate seq num
        auto seq = rd.find( fix_spec::MsgSeqNum );
        if( seq == rd.end( ) ) {
            ctx->log->warn( "missing seq_num, dropping" );
            msg.state = net::server::InvalidSeq;
            return msg;
        }

        /*int cur_seq = seq->val.as_int( );
        
        int srv_seq = session->fix.next_in.load( std::memory_order_acquire );
        if( srv_seq != cur_seq ) {
            ctx->log->warn( "fix message sequence mismatch, dropping, got {}, expected {}", cur_seq, srv_seq );
            msg.state = net::server::InvalidSeq;
            return msg;
        }

        session->fix.next_in.fetch_add( 1, std::memory_order_release );*/

        return msg;
    }
};

struct post_read_node_t {
    net::server::ctx_ptr ctx;
    post_read_node_t( net::server::ctx_ptr &c ) : ctx( c ) {}

    void operator( )( net::server::session_message_t msg ) {
        if( !msg.session ) {
            ctx->log->critical( "invalid session pointer" );
            return;
        }

        ctx->log->debug( "releasing {:x}", uintptr_t( msg.buf ) );
        ctx->bufpool.release( msg.buf );

        // check session status
        if( msg.state == net::server::InvalidSeq or msg.state == net::server::InvalidFix ) {
            msg.session->close( );
            ctx->dealloc_session( msg.session );
        }
    }
};

int main( ) {
    net::server::tcp_server_t< on_read_node_t, post_read_node_t > srv( HOST, PORT );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    srv.ctx->log->info( "serving on {}:{}", HOST, PORT );

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};