#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;

	namespace cb {
		void on_poll( uv_poll_t *handle, int status, int flags );
	}; // namespace cb

	struct client_context_t {
		std::unordered_map< int, std::shared_ptr< tcp_session > > targets;

		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 256 > in_queue;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 256 > out_queue;
	};

	class tcp_client {
	  private:
		uv_loop_t m_loop;

	  protected:
		std::shared_ptr< client_context_t > m_ctx;
		details::log_ptr_t m_log;

	  public:
		tcp_client( const std::string_view log_name, bool to_file )
			: m_ctx{ std::make_shared< client_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) } {
			uv_loop_init( &m_loop );
			m_loop.data = this;

			m_log->set_level( spdlog::level::debug );
		}

		auto &ctx( ) { return m_ctx; }
		auto &log( ) { return m_log; }

		int connect( const std::string_view host, const std::string_view port );

		int run( ) { return uv_run( &m_loop, UV_RUN_DEFAULT ); }
	};

}; // namespace net