#include "include.h"

#include <net.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

struct on_read_node_t {
    net::server::ctx_ptr ctx;
    on_read_node_t( net::server::ctx_ptr &c ) : ctx( c ) {}

    net::server::session_message_t operator( )( net::server::session_message_t msg ) {
        std::string_view buf( msg.buf, msg.len - 1 );
        ctx->log->info( "ptr: {:x}, size: {}", uintptr_t( msg.buf ), buf.size( ) );

        if ( !fix::is_valid_fix( buf ) ) {
            ctx->log->warn( "invalid FIX message, dropping..." );
            msg.status = net::server::drop;
            return msg;
        }

        auto session = ctx->sessions.at( msg.sock );

        const fix::fix_message_t rd{ buf };

        const auto it = rd.find( fix_spec::tag::MsgType );
        if ( !it->tag ) {
            ctx->log->warn( "missing fix message type." );
            msg.status = net::server::drop;

            return msg;
        }

        const auto val = it->val.as_str( );
        if ( memcmp( val.data( ), fix_spec::MsgType_val::LOGON, sizeof( fix_spec::MsgType_val::LOGON ) ) == 1 ) {
            if ( session->fix_session.state & net::server::LogOn ) {
                ctx->log->warn( "got logon request on logged in session, dropping..." );
                msg.status = net::server::drop;
                return msg;
            }

            // set heartbeat, senderid, targetid
            for ( auto &f : rd ) {
                if ( f.tag == fix_spec::tag::TargetCompID ) { // check if valid against local compid
                    session->fix_session.targetid = f.val.as_str( );
                    continue;
                }

                if ( f.tag == fix_spec::tag::SenderCompID ) {
                    session->fix_session.senderid = f.val.as_str( );
                    continue;
                }
            }

            session->fix_session.state |= net::server::LogOn;
            ctx->log->info( "client {} logged in to FIX session, senderid {}", msg.sock, session->fix_session.senderid );

            // send confirmation

            return msg;
        }

        if( !( session->fix_session.state & net::server::LogOn ) ) {
            ctx->log->info( "got FIX msg from unknown client, dropping..." );
            msg.status = net::server::drop;
            return msg;
        }

        if ( memcmp( val.data( ), fix_spec::MsgType_val::SEQUENCE_RESET,
                     sizeof( fix_spec::MsgType_val::SEQUENCE_RESET ) ) ) {
            session->fix_session.seq = 0;
        }

        return msg;
    }
};

struct post_read_node_t {
    net::server::ctx_ptr ctx;
    post_read_node_t( net::server::ctx_ptr &c ) : ctx( c ) {}

    void operator( )( net::server::session_message_t msg ) {
        ctx->bufpool.release( msg.buf );
        switch ( msg.status ) {
            case net::server::drop: {
                auto it = ctx->sessions.find( msg.sock );
                if ( it != ctx->sessions.end( ) ) {
                    ctx->log->warn( "dropping {}", msg.sock );
                    it->second->close( );
                    ctx->sessions.erase( it );
                }
            } break;
            default:
                break;
        }
    }
};

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

void on_timer_cb( uv_timer_t *handle ) {
    auto ctx = ( net::server::server_context_t * )handle->data;
    if ( ctx->sessions.empty( ) ) {
        return;
    }

    for ( auto &[ sock, session ] : ctx->sessions ) {
        int ret = session->write( fix_buf );

        // ctx->log->debug( "sent {} bytes to {}", ret, sock );
    }
}

int main( ) {
    net::server::tcp_server_t< on_read_node_t, post_read_node_t > srv( HOST, PORT );

    if ( srv.bind( ) != 0 ) {
        srv.ctx->log->error( "failed to bind" );
        return 0;
    }

    srv.ctx->log->info( "serving on {}:{}", HOST, PORT );

    // uv_timer_t timer;
    // srv.register_loop_timer( &timer, on_timer_cb, 10 );

    srv.ctx->log->info( "listening..." );

    return srv.run( );
};