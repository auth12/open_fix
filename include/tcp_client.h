#pragma once

#include "include.h"
#include "details.h"
#include "session.h"
#include "message.h"
#include "errors.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/poll.h>

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;
	static constexpr int max_targets = 64;

	struct tcp_client_ctx_t {
		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;
		std::vector< poll_t > targets;

		atomic_queue::AtomicQueue2< message::net_msg_t, 1024, true, true, true, true > in_queue;
		atomic_queue::AtomicQueue2< message::net_msg_t, 1024 > out_queue;

		details::log_ptr_t log;

		tcp_client_ctx_t( const std::string_view log_name, bool to_file )
			: log{ details::log::make_sync( log_name, to_file ) } {
			log->set_level( spdlog::level::debug );
		}
	};

	class tcp_client {
	  private:
		std::unique_ptr< tcp_client_ctx_t > m_ctx;

	  public:
		tcp_client( const std::string_view log_name, bool to_file )
			: m_ctx{ std::make_unique< tcp_client_ctx_t >( log_name, to_file ) } {}

		tcp_client( const tcp_client & ) = delete;
		tcp_client &operator=( const tcp_client & ) = delete;

		tcp_client( tcp_client && ) = delete;
		tcp_client &operator=( tcp_client && ) = delete;

		auto &ctx( ) { return m_ctx; }

		int connect( const std::string_view host, const std::string_view port );

		void run( );
	};

}; // namespace net