#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;

	namespace client_cb {
		void on_read( uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf );
		void on_buf_alloc( uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf );
		void on_connect( uv_connect_t *req, int status );
	}; // namespace client_cb

	struct tcp_client_context_t {
		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;

		tbb::concurrent_vector< std::shared_ptr< tcp_session > > targets;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 1024 > msg_queue;

		tcp_client_context_t( ) = default;
	};

	class tcp_client {
	  public:
		tcp_client( const std::string_view log_name, bool to_file, const unsigned int msg_queue_elements = 1024 );

		int connect( const std::string_view host, const uint16_t port );

		int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

		auto &log( ) { return m_log; }
		auto &ctx( ) { return m_ctx; }
		auto *loop( ) { return &m_loop; }

		void set_on_connect( std::function< void( tcp_session * ) > &&fn ) { m_on_connect.swap( fn ); }

		auto &con_fn( ) { return m_on_connect; }

	  private:
		uv_loop_t m_loop;

		std::shared_ptr< tcp_client_context_t > m_ctx;

		std::function< void( tcp_session * ) > m_on_connect;

		details::log_ptr_t m_log;
	};

}; // namespace net