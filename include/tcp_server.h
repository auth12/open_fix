#pragma once

#include "errors.h"
#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"

#include <sys/poll.h>
#include <sys/ioctl.h>

namespace net {
	static constexpr int server_bufpool_elements = 256;
	static constexpr int server_buf_size = 1024;
	static constexpr int max_clients = 64;

	struct tcp_server_context_t {
		details::log_ptr_t log;
		details::object_pool< char, server_buf_size, server_bufpool_elements > bufpool;
		atomic_queue::AtomicQueue2< message::net_msg_t, 1024, true, true, true, true > in_queue;
		std::vector< poll_t > targets;

		tcp_server_context_t( const std::string_view log_name, const bool to_file )
			: log{ details::log::make_sync( log_name, to_file ) } {
			log->flush_on( spdlog::level::n_levels );
			log->set_level( spdlog::level::debug );
			log->set_pattern( "[%t]%+" );
		}
	};

	class tcp_server {
	  private:
		std::unique_ptr< tcp_server_context_t > m_ctx;
		tcp_session m_listen_session;

	  public:
		tcp_server( const std::string_view log_name, const bool to_file )
			: m_ctx{ std::make_unique< tcp_server_context_t >( log_name, to_file ) } {}

		int bind( const std::string_view host, const std::string_view port );
		void run( );
		auto &ctx( ) { return m_ctx; }
	};
}; // namespace net