#pragma once

#include "include.h"

#include "tcp_client.h"

#include "fix_parse.h"

#include "config.h"

namespace fix {
	enum fix_session_state : int { Offline = 0, Connected, LoggedIn };

	struct fix_msg_t {
		// fix specific
		uint8_t type, checksum = 0;

		int64_t timestamp;

		char *ptr;
		bool is_final;
	};

	template < size_t InSize > struct fix_handler {
		atomic_queue::AtomicQueue2< fix_msg_t, InSize, true, true, false, true > in_queue;
		TSCNS clock;

		fix_handler( ) {
			clock.init( );
			clock.calibrate( );
		}

		bool operator( )( net::tcp_session *session, char *buf, size_t len ) {
			fix::fix_message_t msg{ buf };

			msg.init( );
			if ( !msg.valid ) {
				spdlog::error( "Invalid FIX message received." );
				return true;
			}

			ptrdiff_t loss = ( buf + len ) - msg.end_;

			in_queue.push( fix_msg_t{ msg.type, msg.checksum, clock.rdtsc( ), buf, loss == 0 } );

			if ( loss > 0 ) {
				spdlog::warn( "TCP buffer does not contain single FIX message, parsing..." );

				for ( auto i = msg.end_; i < buf + len; i = msg.end_ ) {
					msg.begin_ = i;
					msg.init( );
					if ( !msg.valid ) {
						spdlog::error( "Invalid message appended, increase buffer size." );
						return true;
					}

					loss = ( buf + len ) - msg.end_;

					spdlog::debug( "{:x} -> {:x}, loss: {}", uintptr_t( msg.begin_ ), uintptr_t( msg.end_ ), loss );

					in_queue.push( fix_msg_t{ msg.type, msg.checksum, clock.rdtsc( ), buf, loss == 0 } );
				}
			}

			return false;
		}
	};

	constexpr size_t buffer_size = 2048;

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

			const auto latency = m_handler.clock.rdns( ) - m_handler.clock.tsc2ns( msg.timestamp );

			m_log->debug( "IN:{} -> buf: {:x}, latency: {}ns", m_broker.target_id, uintptr_t( msg.ptr ),
						  latency );
			m_log->info( "IN:{} -> type: {}, checksum: {}", m_broker.target_id, ( char )msg.type, msg.checksum );

			if ( msg.type == '0' or msg.type == '1' ) {
				m_log->info( "Posting heartbeat on {} session", m_broker.target_id );
				post_heartbeat( );
				return true;
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
			auto ts1 = m_handler.clock.rdtsc( );
			size_t len = m_broker.create_logon( buf, buffer_size );
			auto ts2 = m_handler.clock.rdtsc( );

			m_log->debug( "Logon creation time: {}ns", m_handler.clock.tsc2ns( ts2 ) - m_handler.clock.tsc2ns( ts1 ) );

			post( net::tcp_msg_t{ buf, len } );
		}

		void post_heartbeat( char *buf = nullptr ) {
			if ( !buf )
				buf = get_buf( );

			if ( !buf ) {
				m_log->error( "Failed to get buffer for heartbeat message." );
				return;
			}

			auto ts1 = m_handler.clock.rdtsc( );
			size_t len = m_broker.create_heartbeat( buf, buffer_size );
			auto ts2 = m_handler.clock.rdtsc( );

			m_log->debug( "HB creation time: {}ns", m_handler.clock.tsc2ns( ts2 ) - m_handler.clock.tsc2ns( ts1 ) );

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
