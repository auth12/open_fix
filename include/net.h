#include "include.h"

#include <details.h>

#define BUF_LEN 1024

namespace net {
    struct msg_t {
        details::buf_t buf;

        msg_t( ) = default;
    };

    void on_poll( uv_poll_t *handle, int status, int flags );

    struct client_context_t {
        mbedtls_net_context sock;

        details::log_ptr_t log;

        uv_poll_t poll_handle;

        details::bufpool_t< BUF_LEN, 256 > bufpool;

        tbb::flow::graph g;
        tbb::flow::queue_node< msg_t > queue;

        client_context_t( std::string_view log_name ) : log{ details::log::make_sync( log_name.data( ) ) }, queue{ g } {
            mbedtls_net_init( &sock );

            log->flush_on( spdlog::level::n_levels );
            log->set_level( spdlog::level::debug );
            log->set_pattern( "[%t]%+" );
        }

        void add_queue_edge( tbb::flow::function_node< msg_t > &node ) { tbb::flow::make_edge( queue, node ); }

        int write( const char *data, const size_t size ) {
            return mbedtls_net_send( &sock, ( unsigned char * )data, size );
        }
        int read( char *buf, const size_t len = BUF_LEN ) {
            return mbedtls_net_recv( &sock, ( unsigned char * )buf, len );
        }

        void close( ) {
            mbedtls_net_free( &sock );
            uv_poll_stop( &poll_handle );
        }
    };

    template < typename OnMsg > struct tcp_client_t {
        std::shared_ptr< client_context_t > ctx;
        uv_loop_t loop;

        std::string host, port;

        tbb::flow::function_node< msg_t > on_msg_node;

        tcp_client_t( const std::string_view _host, const std::string_view _port )
            : ctx{ std::make_shared< client_context_t >( _host ) }, host{ _host }, port{ _port },
              on_msg_node{ ctx->g, tbb::flow::unlimited, OnMsg( ctx ) } {
            uv_loop_init( &loop );
            uv_loop_configure( &loop, UV_LOOP_BLOCK_SIGNAL );

            ctx->add_queue_edge( on_msg_node );

            loop.data = ctx.get( );
        }

        int connect( ) {
            ctx->log->info( "attempting to connect to {}:{}", host, port );
            int ret = mbedtls_net_connect( &ctx->sock, host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
            if ( ret != 0 ) {
                ctx->log->error( "failed to connect to server, return: {}", ret );
                return ret;
            }

            ctx->log->info( "connected." );

            ret = uv_poll_init( &loop, &ctx->poll_handle, ctx->sock.fd );
            ret = uv_poll_start( &ctx->poll_handle, UV_READABLE, on_poll );

            return ret;
        }

        int run( ) { return uv_run( &loop, UV_RUN_DEFAULT ); }
    };

    namespace server {
        struct session_message_t {
            char *buf = nullptr;
            size_t len;
            int sock;
        };

        struct session_t {
            uv_poll_t handle;
            mbedtls_net_context sock;
            
            session_t( ) { mbedtls_net_init( &sock ); }

            int write( const std::string_view buf ) {
                return mbedtls_net_send( &sock, ( const unsigned char * )buf.data( ), buf.size( ) );
            }

            void close( ) {
                uv_poll_stop( &handle );
                mbedtls_net_close( &sock );
                mbedtls_net_free( &sock );
            }
        };

        struct server_context_t {
            details::bufpool_t< 2048, 512 > bufpool;

            details::log_ptr_t log;

            mbedtls_net_context sock;

            tbb::flow::graph g;
            tbb::flow::queue_node< session_message_t > in_q;

            std::unordered_map< int, std::shared_ptr< session_t > > sessions;

            server_context_t( const std::string_view log_name )
                : log{ details::log::make_sync( log_name.data( ) ) }, in_q{ g } {
                mbedtls_net_init( &sock );

                log->flush_on( spdlog::level::n_levels );
                log->set_level( spdlog::level::debug );
                log->set_pattern( "[%t]%+" );
            }

            void add_queue_edge( tbb::flow::function_node< session_message_t > &node ) {
                tbb::flow::make_edge( in_q, node );
            }
        };

        void on_connect( uv_poll_t *handle, int status, int events );
        void on_read( uv_poll_t *handle, int status, int events );

        template < typename OnClientMsg > struct tcp_server_t {
            std::shared_ptr< server_context_t > ctx;

            uv_loop_t loop;
            uv_poll_t handle;

            struct sockaddr_in addr;

            std::string host, port;

            tbb::flow::function_node< session_message_t > on_msg_node;

            tcp_server_t( const std::string_view h_, const std::string_view p_ )
                : ctx{ std::make_unique< server_context_t >( h_ ) }, host{ h_ }, port{ p_ },
                  on_msg_node{ ctx->g, tbb::flow::unlimited, OnClientMsg( ctx ) } {
                uv_loop_init( &loop );
                uv_loop_configure( &loop, UV_LOOP_BLOCK_SIGNAL );

                ctx->add_queue_edge( on_msg_node );

                loop.data = ctx.get( );
            }

            int bind( ) {
                int ret = mbedtls_net_bind( &ctx->sock, host.c_str( ), port.c_str( ), MBEDTLS_NET_PROTO_TCP );
                if ( ret != 0 ) {
                    return ret;
                }

                // setup poll
                uv_poll_init_socket( &loop, &handle, ctx->sock.fd );

                uv_poll_start( &handle, UV_READABLE, on_connect );

                return 0;
            }

            template< typename Fn >
            void register_loop_timer( uv_timer_t *handle, const Fn &&cb, const int interval_ms  ) {
                uv_timer_init( &loop, handle );
                handle->data = ctx.get( );
                uv_timer_start( handle, cb, 0, interval_ms );
            }

            void stop_timer( uv_timer_t *handle ) {
                uv_timer_stop( handle );
            }

            int run( ) { return uv_run( &loop, UV_RUN_DEFAULT ); }
        };
    }; // namespace server
};     // namespace net