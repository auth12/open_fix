#pragma once

#include "include.h"

#include "tcp_client.h"

#include "fix_parse.h"

#include "config.h"

namespace fix {
	enum fix_session_state : int { Offline = 0, Connected, LoggedIn };

	struct fix_msg_t {
		int64_t timestamp;
		uint8_t type, checksum;
		bool valid;
		char *buf;
		size_t len, loss;
	};

	template < size_t InSize > struct fix_handler {
		atomic_queue::AtomicQueue2< fix_msg_t, InSize, true, true, false, true > in_queue;
		TSCNS clock;

		fix_handler( ) {
			clock.init( );
			clock.calibrate( );
		}

		bool operator( )( net::tcp_session *session, char *buf, size_t len ) {
			fix::fix_message_t msg{ buf, len };

			msg.init( );

			in_queue.push( fix_msg_t{ clock.rdtsc( ), msg.type, msg.checksum, msg.valid, buf, len, msg.loss } );

			return false;
		}
	};

	constexpr size_t buffer_size = 512;

	using fix_tcp_client = net::tcp_client_impl< buffer_size, 64, 64, fix_handler< 64 > >;

	template < class Broker > class fix_client : public fix_tcp_client {
	  private:
		Broker m_broker;
		details::log_ptr_t m_log;

	  public:
		fix_client( ) : m_log{ details::log::make_sync( "FIX" ) }, fix_tcp_client{ "TCP" }, m_broker{ } {
			m_log->set_pattern( LOG_PATTERN );
			m_log->set_level( spdlog::level::debug );

			m_broker.state = Offline;
		}

		bool consume_fix_msg( fix_msg_t &msg ) {
			if ( !m_handler.in_queue.try_pop( msg ) ) {
				return false;
			}

			if ( !msg.valid ) {
				m_log->error( "Invalid fix message received on {} session.", m_broker.target_id );
				return false;
			}

			const auto latency = m_handler.clock.rdns( ) - m_handler.clock.tsc2ns( msg.timestamp );

			m_log->debug( "IN:{} -> buf: {:x}, len: {}, latency: {}ns", m_broker.target_id, uintptr_t( msg.buf ),
						  msg.len, latency );
			m_log->info( "IN:{} -> type: {}, checksum: {}, loss: {}\n\t{}", m_broker.target_id, ( char )msg.type,
						 msg.checksum, msg.loss, std::string_view{ msg.buf, msg.len } );

			if ( msg.type == '0' or msg.type == '1' ) {
				m_log->info( "Posting heartbeat on {} session", m_broker.target_id );
				post_heartbeat( msg.buf );
				return false;
			}

			if ( msg.type == 'A' ) {
				m_broker.state = LoggedIn;
				m_log->info( "Logged in to {}.", m_broker.target_id );
			}

			return true;
		}

		auto *clock( ) { return &m_handler.clock; }
		auto *broker( ) { return &m_broker; }

		void post_logon( ) {
			auto buf = get_buf( );
			if ( !buf ) {
				m_log->error( "Failed to get buffer for logon message." );
				return;
			}

			size_t len = m_broker.create_logon( buf, buffer_size );

			post( net::tcp_msg_t{ buf, len } );
		}

		void post_heartbeat( char *buf = nullptr ) {
			if ( !buf )
				buf = get_buf( );

			if ( !buf ) {
				m_log->error( "Failed to get buffer for heartbeat message." );
				return;
			}

			size_t len = m_broker.create_heartbeat( buf, buffer_size );

			post( net::tcp_msg_t{ buf, len } );
		}

		void post_market_request( char *buf = nullptr ) {
			if ( !buf )
				buf = get_buf( );

			if ( !buf ) {
				m_log->error( "Failed to get buffer for heartbeat message." );
				return;
			}

			size_t len = m_broker.create_market_request( buf, buffer_size );

			post( net::tcp_msg_t{ buf, len } );
		}
	};
}; // namespace fix