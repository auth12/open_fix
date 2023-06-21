#pragma once

#include "include.h"

namespace net {
	enum session_state : int { Offline = 0, Idle, MaxNetstate };

	class tcp_session {
	  public:
		tcp_session( ) : m_state{ Offline } {}

		tcp_session( const tcp_session & ) = delete;
		tcp_session( tcp_session && ) = delete;
		tcp_session &operator=( const tcp_session & ) = delete;
		tcp_session &operator=( tcp_session && ) = delete;

		int poll_init( uv_loop_t *loop ) { return uv_poll_init_socket( loop, &m_poll_handle, m_ctx.fd ); }

		template < typename Fn > int poll_start( const Fn &&on_poll, const int events ) {
			return uv_poll_start( &m_poll_handle, events, on_poll );
		}

		int write( const void *data, const size_t len ) {
			return mbedtls_net_send( &m_ctx, ( const unsigned char * )data, len );
		}

		int write( const std::string_view &buf ) { return write( buf.data( ), buf.size( ) ); }

		void close( ) {
			mbedtls_net_close( &m_ctx );
			mbedtls_net_free( &m_ctx );

			m_state = Offline;
		}

		inline int fd( ) const { return m_ctx.fd; }

		mbedtls_net_context *net_ctx( ) { return &m_ctx; }
		uv_poll_t *poll_handle( ) { return &m_poll_handle; }
		int poll_stop( ) { return uv_poll_stop( &m_poll_handle ); }

		void set_state( const int &state ) { m_state = state; }

		auto &state( ) { return m_state; }

	  private:
		mbedtls_net_context m_ctx;
		uv_poll_t m_poll_handle;

		std::atomic_int m_state;
	};
}; // namespace net
