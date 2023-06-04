#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

namespace net {
    static constexpr int cli_bufpool_elements = 256;
    static constexpr int cli_buf_size = 1024;

    void on_target_poll( uv_poll_t *handle, int status, int flags );

    struct tcp_client_context_t {
        details::bufpool_t< cli_buf_size, cli_bufpool_elements > bufpool;

        tbb::concurrent_unordered_map< int, std::shared_ptr< tcp_session > > targets;

        tcp_client_context_t( ) = default;
    };

    class tcp_client {
      public:
        tcp_client( const std::string_view log_name, bool to_file )
            : m_ctx{ std::make_shared< tcp_client_context_t >( ) },
              m_log{ details::log::make_sync( log_name, to_file ) }, m_message_queue{ m_message_graph } {
            m_log->flush_on( spdlog::level::n_levels );
            m_log->set_level( spdlog::level::debug );
            m_log->set_pattern( "[%t]%+" );

            uv_loop_init( &m_loop );
            m_loop.data = this;
        }

        int connect( const std::string_view host, const std::string_view port ) {
            auto session = std::make_shared< tcp_session >( );

            int ret = mbedtls_net_connect( session->net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
            if ( ret != 0 ) {
                return ret;
            }

            ret = session->poll_init( &m_loop );
            if ( ret != 0 ) {
                session->close( );
                return ret;
            }

            session->poll_handle( )->data = session.get( );

            session->set_state( Idle );

            ret = session->poll_start( on_target_poll, UV_READABLE );
            if ( ret != 0 ) {
                session->close( );
                return ret;
            }

            m_ctx->targets[ session->fd( ) ].swap( session );

            return 0;
        }

        int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

        auto &log( ) { return m_log; }
        auto &ctx( ) { return m_ctx; }
        auto &queue_node( ) { return m_message_queue; }
        auto &graph( ) { return m_message_graph; }

        auto *loop( ) { return &m_loop; }

      private:
        uv_loop_t m_loop;

        tbb::flow::graph m_message_graph;
        tbb::flow::queue_node< message::net_msg_t > m_message_queue;

        std::shared_ptr< tcp_client_context_t > m_ctx;

        details::log_ptr_t m_log;
    };

}; // namespace net