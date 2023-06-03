#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>

namespace net {
    using net_msg_t = tbb::flow::tagged_msg< int, std::span< char > >;

    void on_server_poll( uv_poll_t *handle, int status, int events );
    void on_session_poll( uv_poll_t *handle, int status, int events );

    struct tcp_server_context_t {
        details::log_ptr_t log;

        details::bufpool_t< 1024, 128 > bufpool;
        tbb::concurrent_unordered_map< int, std::shared_ptr< tcp_session > > sessions;

        tcp_server_context_t( const std::string_view log_name, const bool to_file )
            : log{ details::log::make_sync( log_name, to_file ) } {
            log->flush_on( spdlog::level::n_levels );
            log->set_level( spdlog::level::debug );
            log->set_pattern( "[%t]%+" );
        }
    };

    class tcp_server {
      public:
        tcp_server( const std::string_view log_name, const bool to_file )
            : m_ctx{ std::make_shared< tcp_server_context_t >( log_name, to_file ) },
              m_message_queue{ m_message_graph } {
            uv_loop_init( &m_loop );
            m_loop.data = this;
        }

        int bind( const std::string_view host, const std::string_view port ) {
            int ret =
                mbedtls_net_bind( m_server_session.net_ctx( ), host.data( ), port.data( ), MBEDTLS_NET_PROTO_TCP );
            if ( ret != 0 ) {
                return ret;
            }

            ret = m_server_session.poll_init( &m_loop );
            if ( ret != 0 ) {
                return ret;
            }

            return m_server_session.poll_start( on_server_poll, UV_READABLE );
        }

        int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

        auto &ctx( ) { return m_ctx; }

        tcp_session &session( ) { return m_server_session; }

        auto &queue_node( ) { return m_message_queue; }
        auto &graph( ) { return m_message_graph; }

      private:
        uv_loop_t m_loop;
        tcp_session m_server_session; // server socket

        tbb::flow::graph m_message_graph;
        tbb::flow::queue_node< net_msg_t > m_message_queue;

        std::shared_ptr< tcp_server_context_t > m_ctx;
    };
}; // namespace net