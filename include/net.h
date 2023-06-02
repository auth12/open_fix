#include "include.h"

#include "details.h"

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>

#define BUF_LEN 1024
#define BUF_POOL_ELEMENTS 4096

namespace net {
    namespace cli {
        struct msg_t {
            char *buf = nullptr;
            size_t len = 0;
            int sock = 0;

            msg_t( ) = default;
            msg_t( char *ptr, size_t l, const int &s_ ) : buf{ ptr }, len{ l }, sock{ s_ } {}
        };

        void on_poll( uv_poll_t *handle, int status, int flags );

        struct fix_session_t {
            std::atomic< int > next_in, next_out, state;
        };

        struct server_descriptor_t {
            mbedtls_net_context sock;
            uv_poll_t handle;

            int FIX_minor;
            int FIX_major;

            fix_session_t fix;

            server_descriptor_t( ) { mbedtls_net_init( &sock ); }

            int write( const char *data, const size_t size ) {
                return mbedtls_net_send( &sock, ( unsigned char * )data, size );
            }
            int read( char *buf, const size_t len = BUF_LEN ) {
                return mbedtls_net_recv( &sock, ( unsigned char * )buf, len );
            }

            void reset( ) {
                fix.state = 0;
                fix.next_in = 0;
                fix.next_out = 0;
            }

            void close( ) {
                mbedtls_net_close( &sock );
                mbedtls_net_free( &sock );
                uv_poll_stop( &handle );
            }

            bool operator==( const int &fd ) const { return sock.fd == fd; }
        };

        struct desc_fd_compare_t {
            int fd;

            desc_fd_compare_t( int fd_ ) : fd{ fd_ } {}
            bool operator( )( const server_descriptor_t *desc ) { return desc->sock.fd == fd; }
        };

        struct cli_context_t {
            details::log_ptr_t log;

            details::bufpool_t< BUF_LEN, BUF_POOL_ELEMENTS > bufpool;

            tbb::flow::graph g;
            tbb::flow::queue_node< msg_t > queue;

            details::obj_pool_t< server_descriptor_t, 64 > server_pool;

            std::mutex vec_mutex;
            std::vector< server_descriptor_t * > active_targets;

            cli_context_t( std::string_view log_name )
                : log{ details::log::make_sync( log_name.data( ) ) }, queue{ g } {

                log->flush_on( spdlog::level::n_levels );
                log->set_level( spdlog::level::debug );
                log->set_pattern( "[%t]%+" );
            }

            bool remove_target( const int &sock ) {
                auto it = std::find_if( active_targets.begin( ), active_targets.end( ), desc_fd_compare_t( sock ) );
                if ( it != active_targets.end( ) ) {
                    std::lock_guard< std::mutex > lk( vec_mutex );
                    active_targets.erase( it );
                    return true;
                }

                return false;
            }

            void add_queue_edge( tbb::flow::function_node< msg_t > &node ) { tbb::flow::make_edge( queue, node ); };
        };

        template < typename OnMsg > struct tcp_cli_t {
            uv_loop_t loop;

            std::shared_ptr< cli_context_t > ctx;
            tbb::flow::function_node< msg_t > on_msg_node;

            tcp_cli_t( )
                : ctx{ std::make_shared< cli_context_t >( "cli" ) },
                  on_msg_node{ ctx->g, tbb::flow::unlimited, OnMsg( ctx ) } {
                uv_loop_init( &loop );
                uv_loop_configure( &loop, UV_LOOP_BLOCK_SIGNAL );

                ctx->add_queue_edge( on_msg_node );

                loop.data = ctx.get( );
            }

            int connect( const std::string_view host, const std::string_view port, const int fix_maj = 4,
                         const int fix_min = 4 ) {
                ctx->log->info( "attempting to connect to {}:{}", host, port );

                // register new target
                auto desc = ctx->server_pool.get( );
                if( !desc ) {
                    ctx->log->warn( "no available free descriptors" );
                    return 1;
                }
                desc->reset( );

                desc->FIX_major = fix_maj;
                desc->FIX_minor = fix_min;

                int ret = mbedtls_net_connect( &desc->sock, host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
                if ( ret != 0 ) {
                    ctx->log->error( "failed to connect to server, return: {}", ret );
                    ctx->server_pool.release( desc );
                    return ret;
                }

                ctx->log->info( "connected." );

                ret = uv_poll_init( &loop, &desc->handle, desc->sock.fd );
                desc->handle.data = desc;

                ret = uv_poll_start( &desc->handle, UV_READABLE, on_poll );

                ctx->active_targets.emplace_back( desc );

                return ret;
            }

            int run( ) { return uv_run( &loop, UV_RUN_DEFAULT ); }
        };
    } // namespace cli

    namespace server {
        enum message_state : int {
            Ok = 1,
            InvalidFix,
            InvalidSeq,
        };

        enum fix_session_state : int { Idle = 1, LoggedIn };

        struct fix_session_t {
            std::atomic< int > next_in, next_out, state;
        };

        struct session_t {
            uv_poll_t handle;
            mbedtls_net_context sock;

            fix_session_t fix;

            session_t( ) { mbedtls_net_init( &sock ); }

            int write( const std::string_view buf ) {
                return mbedtls_net_send( &sock, ( const unsigned char * )buf.data( ), buf.size( ) );
            }

            void close( ) {
                uv_poll_stop( &handle );
                mbedtls_net_close( &sock );
                mbedtls_net_free( &sock );
            }

            void reset( ) {
                fix.state = 0;
                fix.next_in = 0;
                fix.next_out = 0;
            }
        };

        struct session_message_t {
            char *buf = nullptr;
            session_t *session = nullptr;
            size_t len = 0;
        };

        struct server_context_t {
            mbedtls_net_context sock;

            details::bufpool_t< BUF_LEN, BUF_POOL_ELEMENTS > bufpool;
            details::log_ptr_t log;

            tbb::flow::graph g;
            tbb::flow::queue_node< session_message_t > in_q;
            details::obj_pool_t< session_t, 128 > sessions; // 128 max sessions

            server_context_t( const std::string_view log_name )
                : log{ details::log::make_sync( log_name.data( ) ) }, in_q{ g } {
                mbedtls_net_init( &sock );

                log->flush_on( spdlog::level::n_levels );
                log->set_level( spdlog::level::debug );
                log->set_pattern( "[%t]%+" );
            }

            template < typename T > void add_queue_edge( T &node ) { tbb::flow::make_edge( in_q, node ); }
        };

        using ctx_ptr = std::shared_ptr< server_context_t >;

        void on_connect( uv_poll_t *handle, int status, int events );
        void on_read( uv_poll_t *handle, int status, int events );

        template < typename OnRead > struct tcp_server_t {
            ctx_ptr ctx;

            uv_loop_t loop;
            uv_poll_t handle;

            struct sockaddr_in addr;

            std::string host, port;

            tbb::flow::function_node< session_message_t > on_msg_node;

            tcp_server_t( const std::string_view h_, const std::string_view p_ )
                : ctx{ std::make_unique< server_context_t >( h_ ) }, host{ h_ }, port{ p_ },
                  on_msg_node{ ctx->g, tbb::flow::unlimited, OnRead( ctx ) } {
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

            template < typename Fn >
            void register_loop_timer( uv_timer_t *handle, const Fn &&cb, const int interval_ms ) {
                uv_timer_init( &loop, handle );
                handle->data = ctx.get( );
                uv_timer_start( handle, cb, 0, interval_ms );
            }

            void stop_timer( uv_timer_t *handle ) { uv_timer_stop( handle ); }

            int run( ) { return uv_run( &loop, UV_RUN_DEFAULT ); }
        };
    }; // namespace server
} // namespace net