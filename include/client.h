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

	struct client_context_t {
		using pollfd_t = std::vector< pollfd >;
		pollfd_t targets;

		details::object_pool< char, cli_buf_size, cli_bufpool_elements > bufpool;
		atomic_queue::AtomicQueue2< std::unique_ptr< message::net_msg_t >, 1024 > msg_queue;

		void remove_target( const pollfd_t::const_iterator &it ) {
			close( it->fd );
			targets.erase( it );
		}

		bool remove_target( const int &fd ) {
			auto it =
				std::find_if( targets.begin( ), targets.end( ), [ fd ]( const pollfd &t ) { return t.fd == fd; } );

			if ( it != targets.end( ) ) {
				remove_target( it );
				return true;
			}

			return false;
		}
	};

	enum client_state : int { OFFLINE = 0, CONNECTING, ONLINE };

	class tcp_client {
	  private:
		std::shared_ptr< client_context_t > m_ctx;
		details::log_ptr_t m_log;
		std::atomic_int m_state;

	  public:
		tcp_client( const std::string_view log_name )
			: m_ctx{ std::make_shared< client_context_t >( ) }, m_log{ details::log::make_sync( log_name ) },
			  m_state{ OFFLINE } {
			m_log->set_level( spdlog::level::debug );
		}

		auto &ctx( ) { return m_ctx; }
		auto &log( ) { return m_log; }

		inline int state( ) { return m_state.load( std::memory_order_acquire ); }

		int try_connect( const std::string_view host, const std::string_view port );

		void run( );
	};

}; // namespace net