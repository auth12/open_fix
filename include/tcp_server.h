#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

namespace net {
	static constexpr int server_bufpool_elements = 256;
	static constexpr int server_buf_size = 1024;
	static constexpr int max_clients = 64;

	namespace server_cb {
		void on_connect( uv_poll_t *handle, int status, int events );
		void on_read( uv_poll_t *handle, int status, int events );
	} // namespace server_cb

	struct tcp_server_context_t {
		tcp_session server_session;

		details::object_pool< char, server_buf_size, server_bufpool_elements > bufpool;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 1024 > msg_queue;
		std::unordered_map< int, std::shared_ptr< tcp_session > > sessions;

		tcp_server_context_t( ) = default;
	};

	class tcp_server {
	  public:
		tcp_server( const std::string_view log_name, const bool to_file );

		int bind( const std::string_view host, const std::string_view port );

		int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

		auto &ctx( ) { return m_ctx; }
		auto &log( ) { return m_log; }
		auto *loop( ) { return &m_loop; }
	  private:
		uv_loop_t m_loop;
		details::log_ptr_t m_log;

		std::shared_ptr< tcp_server_context_t > m_ctx;
	};
}; // namespace net