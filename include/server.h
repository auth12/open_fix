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
        tcp_server( const std::string_view log_name, const bool to_file );

        int bind( const std::string_view host, const std::string_view port );

        int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

        auto &ctx( ) { return m_ctx; }
        auto &log( ) { return m_log; }
        auto &queue_node( ) { return m_message_queue; }
        auto &graph( ) { return m_message_graph; }
        auto *loop( ) { return &m_loop; }

      private:
        uv_loop_t m_loop;

        details::log_ptr_t m_log;

        tbb::flow::graph m_message_graph;
        tbb::flow::queue_node< message::net_msg_t > m_message_queue;

        std::shared_ptr< tcp_server_context_t > m_ctx;
    };
}; // namespace net