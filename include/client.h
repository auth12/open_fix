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
        tcp_client( const std::string_view log_name, bool to_file );

        int connect( const std::string_view host, const std::string_view port );

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