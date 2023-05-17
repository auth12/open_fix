#include "include.h"

#include <net.h>

#include <fix_parse.h>

struct on_msg_node_t {
    std::shared_ptr< net::client_context_t > ctx;
    on_msg_node_t( std::shared_ptr< net::client_context_t > &_c ) : ctx{ _c } {}

    void operator( )( const net::msg_t &msg ) {
        std::string_view buf{ msg.buf.ptr, msg.buf.len };

        if ( fix::is_valid_fix( buf ) ) {
            fix::fix_message_t m{ buf };
            auto it = m.find( fix_spec::tag::MDEntryPx );
            if ( it != m.end( ) ) {
                ctx->log->info( "got px update: {}", it->val.as_float( ) );
            }
        }

        ctx->bufpool.release( msg.buf.ptr );
    }
};

int main( ) {
    net::tcp_client_t< on_msg_node_t > cli( "127.0.0.1", "1515" );
    auto &ctx = cli.ctx;

    int ret = cli.connect( );
    if ( ret != 0 ) {
        ctx->log->error( "failed to connect to host." );

        return 0;
    }

    return cli.run( );
}