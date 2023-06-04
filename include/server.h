#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>

namespace net {
    static constexpr int server_bufpool_elements = 256;
    static constexpr int server_buf_size = 1024;

    void on_server_poll( uv_poll_t *handle, int status, int events );
    void on_session_poll( uv_poll_t *handle, int status, int events );

    struct tcp_server_context_t {
        tcp_session server_session;

        details::bufpool_t< server_buf_size, server_bufpool_elements > bufpool;
        tbb::concurrent_unordered_map< int, std::shared_ptr< tcp_session > > sessions;

        tcp_server_context_t( ) = default;
    };

    class tcp_server {
      public:
        tcp_server( const std::string_view log_name, const bool to_file )
            : m_ctx{ std::make_shared< tcp_server_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) },
              m_message_queue{ m_message_graph } {
                m_log->flush_on( spdlog::level::n_levels );
            m_log->set_level( spdlog::level::debug );
            m_log->set_pattern( "[%t]%+" );

            uv_loop_init( &m_loop );
            m_loop.data = this;
        }

        int bind( const std::string_view host, const std::string_view port ) {
            int ret =
                mbedtls_net_bind( m_ctx->server_session.net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
            if ( ret != 0 ) {
                return ret;
            }

            ret = m_ctx->server_session.poll_init( &m_loop );
            if ( ret != 0 ) {
                return ret;
            }

            return m_ctx->server_session.poll_start( on_server_poll, UV_READABLE );
        }

        int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

        auto &ctx( ) { return m_ctx; }
        auto &log( ) { return m_log; }
        auto &queue_node( ) { return m_message_queue; }
        auto &graph( ) { return m_message_graph; }

      private:
        uv_loop_t m_loop;

        details::log_ptr_t m_log;

        tbb::flow::graph m_message_graph;
        tbb::flow::queue_node< message::net_msg_t > m_message_queue;

        std::shared_ptr< tcp_server_context_t > m_ctx;
    };
}; // namespace net