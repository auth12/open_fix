#pragma once

#include "include.h"

#include "tcp_client.h"

#include "fix_parse.h"

#include "config.h"

namespace fix {
	enum fix_session_state : int { Offline = 0, Connected, AwaitingLogon, LoggedIn };

	class fix_client : public net::tcp_client {
	  private:
		std::atomic_int m_state, m_next_out;
		std::string m_target_id, m_sender_id, m_begin_string;

	  public:
		details::log_ptr_t m_log;

		fix_client( const std::string_view log_name, bool to_file,
					spdlog::level::level_enum log_lvl = spdlog::level::debug )
			: net::tcp_client{ "TCP", to_file, log_lvl }, m_log{ details::log::make_sync( log_name, to_file ) },
			  m_state{ Offline }, m_next_out{ 1 }, m_target_id{ "" }, m_sender_id{ "" }, m_begin_string{ "" } {
			m_log->set_level( log_lvl );
			m_log->set_pattern( LOG_PATTERN );
		};

		int next_seq( ) { return m_next_out.fetch_add( 1, std::memory_order_release ); }
		int state( ) const { return m_state.load( std::memory_order_acquire ); }

		std::string_view target_id( ) const { return m_target_id; }
		std::string_view sender_id( ) const { return m_sender_id; }
		std::string_view begin_str( ) const { return m_begin_string; }

		void set_state( const int &state ) { m_state.store( state, std::memory_order_release ); }
		void set_sender_id( const std::string_view id ) { m_sender_id = id; }
		void set_target_id( const std::string_view id ) { m_target_id = id; }
		void set_begin_str( const std::string_view version ) { m_begin_string = fmt::format( "FIX.{}", version ); }

		// set buf and len to 0 to grab a new buffer from the pool
		// the parameters are here to allow for buffer reuse
		void post_heartbeat( char *buf, size_t len ) {
			if ( !buf and len == 0 ) {
				return;
			}

			fix::fix_writer wr{ buf, len };
			wr.push_header( m_begin_string );
			wr.push_field( fix_spec::MsgType, '0' );

			wr.push_field( fix_spec::TargetCompID, m_target_id );
			wr.push_field( fix_spec::SenderCompID, m_sender_id );

			wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
			wr.push_field( fix_spec::MsgSeqNum, next_seq( ) );
			wr.push_trailer( );

			post( buf, wr.size( ) );
		}
	};
}; // namespace fix