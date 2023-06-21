#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;

	namespace client_cb {
		void on_poll( uv_poll_t *handle, int status, int flags );
	}; // namespace client_cb

	struct tcp_client_context_t {
		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;

		tbb::concurrent_unordered_map< int, std::shared_ptr< tcp_session > > targets;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 1024 > msg_queue;

		tcp_client_context_t( ) = default;
	};

	enum callback_type : int { ON_CONNECT = 0, ON_READ, ON_DISCONNECT };

	class tcp_client {
	  public:
		tcp_client( const std::string_view log_name, bool to_file );

		int connect( const std::string_view host, const std::string_view port );

		int run( const uv_run_mode run_mode = UV_RUN_DEFAULT ) { return uv_run( &m_loop, run_mode ); }

		auto &log( ) { return m_log; }
		auto &ctx( ) { return m_ctx; }
		auto *loop( ) { return &m_loop; }

		template < typename Fn > void register_callback( const int type, const Fn &&fn ) {
			m_callbacks[ type ].emplace_back( fn );
		}

	  private:
		std::shared_ptr< tcp_client_context_t > m_ctx;
		uv_loop_t m_loop;

		std::unordered_map< int, std::list< std::function< void( tcp_session * ) > > > m_callbacks;

		details::log_ptr_t m_log;
	};

}; // namespace net