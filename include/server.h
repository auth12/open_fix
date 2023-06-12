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
	static constexpr int max_clients = 64;

	namespace server_cb {
		void on_session_connect( uv_stream_t *server, int status );
		void on_read( uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf );
		void on_buf_alloc( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf );
	} // namespace server_cb

	struct tcp_server_context_t {
		tcp_session server_session;

		details::object_pool< char, server_buf_size, server_bufpool_elements > bufpool;
		
		tbb::concurrent_unordered_map< int, std::shared_ptr< tcp_session > > sessions;

		tcp_server_context_t( ) = default;
	};

	class tcp_server {
	  public:
		tcp_server( const std::string_view log_name, const bool to_file, const unsigned int msg_queue_elements = 1024 );

		int bind( const std::string_view host, const uint16_t port );

		int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

		auto &ctx( ) { return m_ctx; }
		auto &log( ) { return m_log; }
		auto *loop( ) { return &m_loop; }
		auto &msg_queue( ) { return m_queue; }

		auto get_sem( ) { return &m_sem; }

	  private:
		uv_loop_t m_loop;

		details::log_ptr_t m_log;

		uv_sem_t m_sem;
		atomic_queue::AtomicQueueB2< std::unique_ptr< message::net_msg_t > > m_queue;

		std::shared_ptr< tcp_server_context_t > m_ctx;
	};
}; // namespace net