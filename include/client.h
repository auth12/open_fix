#pragma once

#include "include.h"

#include "details.h"

#include "session.h"

#include "message.h"

#include <unistd.h>
#include <sys/poll.h>

namespace net {
	static constexpr int cli_bufpool_elements = 512;
	static constexpr int cli_buf_size = 1024;

	namespace sock {
		static ssize_t write( const int &fd, const void *data, const size_t &len ) { return send( fd, data, len, 0 ); }
		static ssize_t read( const int &fd, void *data, const size_t &len ) { return recv( fd, data, len, 0 ); }
		static void reset( const int &fd ) { close( fd ); }
	}; // namespace sock

	using poll_container_t = std::vector< pollfd, tbb::cache_aligned_allocator< pollfd > >;

	struct client_context_t {
		poll_container_t targets;

		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 256 > in_queue;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 256 > out_queue;
	};

	enum client_state : int { OFFLINE = 0, CONNECTING, ONLINE };

	class tcp_client {
	  private:
		std::shared_ptr< client_context_t > m_ctx;
		details::log_ptr_t m_log;

		bool on_poll( const pollfd &it );

	  public:
		tcp_client( const std::string_view log_name, bool to_file )
			: m_ctx{ std::make_shared< client_context_t >( ) }, m_log{ details::log::make_sync( log_name, to_file ) } {
			m_log->set_level( spdlog::level::debug );
		}

		auto &ctx( ) { return m_ctx; }
		auto &log( ) { return m_log; }

		int try_connect( const std::string_view host, const std::string_view port );

		void run( );
	};

}; // namespace net