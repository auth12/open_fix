#pragma once

#include "include.h"

#include "client.h"

#include "fix_parse.h"

namespace fix {
	enum fix_session_state : int { // extends tcp session state
		LoggedOut = net::session_state::MaxNetstate,
		LoggedIn
	};

	class fix_session: public net::tcp_session {
		public:
		private:

	};

	struct fix_client_context_t {
		tbb::concurrent_unordered_multimap< int, fix_session > active_sessions; // target_id -> fix_session

		int fix_minor, fix_major;
		std::atomic< int > next_in, state;

		std::string sender_id, target_id;
	};

	class fix_client : public net::tcp_client {
	  public:
		fix_client( const std::string_view log_name, bool to_file )
			: net::tcp_client{ log_name, to_file, 1024 }, m_fix_ctx{ std::make_shared< fix_client_context_t >( ) } { };

		auto &fix_ctx( ) { return m_fix_ctx; }

	  private:
		std::shared_ptr< fix_client_context_t > m_fix_ctx;
	};
}; // namespace fix