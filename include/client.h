#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

#include "errors.h"

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;

	namespace cb {
		void on_poll( uv_poll_t *handle, int status, int flags );
	}; // namespace cb

	struct tcp_client_ctx_t {
		std::mutex targets_m;
		std::vector< tcp_session > targets;

		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;

		atomic_queue::AtomicQueue2< message::net_msg_t, 256 > in_queue;
		atomic_queue::AtomicQueue2< message::net_msg_t, 256 > out_queue;

		details::log_ptr_t log;

		tcp_client_ctx_t( const std::string_view log_name, bool to_file )
			: log{ details::log::make_sync( log_name, to_file ) } {
			log->set_level( spdlog::level::debug );
		}

		bool erase_target( const int fd ) {
			std::scoped_lock< std::mutex > lk( targets_m );
			auto it = std::find( targets.begin( ), targets.end( ), fd );
			if ( it != targets.end( ) ) {
				targets.erase( it );
				return true;
			}
			return false;
		}
	};

	class tcp_client {
	  private:
		std::shared_ptr< tcp_client_ctx_t > m_ctx;

	  protected:
		uv_loop_t m_loop;

	  public:
		tcp_client( const std::string_view log_name, bool to_file )
			: m_ctx{ std::make_shared< tcp_client_ctx_t >( log_name, to_file ) } {
			uv_loop_init( &m_loop );
			m_loop.data = m_ctx.get( );
		}

		tcp_client( const tcp_client & ) = delete;
		tcp_client &operator=( const tcp_client & ) = delete;

		tcp_client( tcp_client && ) = delete;
		tcp_client &operator=( tcp_client && ) = delete;

		auto ctx( ) { return m_ctx; }

		int connect( const std::string_view host, const std::string_view port );

		int run( ) { return uv_run( &m_loop, UV_RUN_DEFAULT ); }
	};

}; // namespace net