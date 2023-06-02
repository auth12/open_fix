#include "include.h"

#include <net.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    net::server::ctx_ptr ctx;
    on_read_node_t( net::server::ctx_ptr &c ) : ctx( c ) {}

    void operator( )( net::server::session_message_t msg ) {
        std::string_view buf( msg.buf, msg.len );
        auto session = msg.session;

        auto tp = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::steady_clock::now( ).time_since_epoch( ) );
        ctx->log->info( "time received: {}", tp.count( ) );

        ctx->log->debug( "ptr: {:x}, size: {}", uintptr_t( msg.buf ), buf.size( ) );

        // pre parser validation
        if ( !fix::is_valid_fix( buf ) ) {
            ctx->log->warn( "invalid FIX message, dropping" );

            msg.session->close( );

            ctx->log->debug( "releasing session {:x}", uintptr_t( msg.session ) );
            ctx->sessions.release( msg.session );

            ctx->bufpool.release( msg.buf );

            return;
        }

        const fix::fix_message_t rd{ buf };

        // validate seq num
        auto seq = rd.find( fix_spec::MsgSeqNum );
        if( seq == rd.end( ) ) {
            ctx->log->warn( "missing seq_num, dropping" );
            msg.session->close( );

            ctx->log->debug( "releasing session {:x}", uintptr_t( msg.session ) );
            ctx->sessions.release( msg.session );

            ctx->bufpool.release( msg.buf );

            return;
        }

        int cur_seq = seq->val.as_int( );
        
        int srv_seq = session->fix.next_in.load( );
        if( srv_seq != cur_seq ) {
            ctx->log->warn( "fix message sequence mismatch, dropping, got {}, expected {}", cur_seq, srv_seq );
            msg.session->close( );

            ctx->log->debug( "releasing session {:x}", uintptr_t( msg.session ) );
            ctx->sessions.release( msg.session );

            ctx->bufpool.release( msg.buf );

            return;
        }

        session->fix.next_in.fetch_add( 1 );

        ctx->bufpool.release( msg.buf );
    }
};

int main( ) {
    net::server::tcp_server_t< on_read_node_t > srv( HOST, PORT );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    srv.ctx->log->info( "serving on {}:{}", HOST, PORT );

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};