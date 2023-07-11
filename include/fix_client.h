#pragma once

#include "include.h"

#include "tcp_client.h"

#include "fix_parse.h"

#include "config.h"

namespace fix {
	enum fix_session_state : int { // extends tcp session state
		LoggedOut = 0,
		AwaitingLogon,
		LoggedIn
	};

	class fix_session {
	  public:
		fix_session( )
			: m_state{ LoggedOut }, m_next_out{ 1 }, m_target_id{ "" }, m_sender_id{ "" }, m_begin_string{ "" } { };

		fix_session( const std::string_view p_target_id, const std::string_view p_sender_id,
					 const std::string_view p_begin_string )
			: m_state{ AwaitingLogon }, m_begin_string{ p_begin_string }, m_sender_id{ p_sender_id },
			  m_target_id{ p_target_id }, m_next_out{ 1 } { };

		int next_seq( ) {
			int ret = m_next_out.load( std::memory_order_acquire );
			m_next_out.fetch_add( 1, std::memory_order_release );
			return ret;
		}

		int state( ) const { return m_state.load( std::memory_order_acquire ); }
		void set_state( const int &state ) { m_state.store( state, std::memory_order_release ); }

		auto &target_id( ) const { return m_target_id; }

		auto &sender_id( ) const { return m_sender_id; }

		auto &begin_str( ) const { return m_begin_string; }

		void set_sender_id( const std::string_view id ) { m_sender_id = id; }

		void set_target_id( const std::string_view id ) { m_target_id = id; }

	  private:
		std::atomic_int m_state, m_next_out;
		std::string m_target_id, m_sender_id, m_begin_string;
	};

	struct fix_client_context_t {
		std::unordered_map< int, std::shared_ptr< fix_session > > active_sessions;

		details::log_ptr_t log;

		fix_client_context_t( const std::string_view log_name, bool to_file )
			: log{ details::log::make_sync( log_name, to_file ) } {
			log->set_level( spdlog::level::debug );
		}
	};

	class fix_client : public net::tcp_client {
	  public:
		fix_client( const std::string_view log_name, bool to_file )
			: net::tcp_client{ "TCP", to_file },
			  m_fix_ctx{ std::make_shared< fix_client_context_t >( log_name, to_file ) } { };

		fix_client( const config::cli_fix_cfg_t &cfg )
			: net::tcp_client{ "TCP", cfg.to_file },
			  m_fix_ctx{ std::make_shared< fix_client_context_t >( cfg.log_name, cfg.to_file ) }, m_cfg{ cfg } { };

		auto &fix_ctx( ) { return m_fix_ctx; }
		auto &cfg( ) { return m_cfg; }

		int connect( );

	  private:
		std::shared_ptr< fix_client_context_t > m_fix_ctx;
		config::cli_fix_cfg_t m_cfg;
	};
}; // namespace fix