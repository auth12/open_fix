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

		int write( const void *data, const size_t len ) {
			uv_buf_t buf{ ( char * )data, len };
			return uv_try_write( ( uv_stream_t * )&m_tcp_handle, &buf, 1 );
		}

		int write( const std::string_view &buf ) { return write( buf.data( ), buf.size( ) ); }

		int init_handle( uv_loop_t *loop ) { return uv_tcp_init( loop, &m_tcp_handle ); }

		void close( ) {
			m_state = Offline;
			m_fd = -1;
			uv_close( ( uv_handle_t * )&m_tcp_handle, nullptr );
		}

		inline int fd( ) const { return m_fd; }

		void set_fd( const int fd ) { m_fd = fd; }

		uv_tcp_t *tcp_handle( ) { return &m_tcp_handle; }

		void set_state( const int &state ) { m_state = state; }

		auto &state( ) { return m_state; }

		auto *connect_request( ) { return &m_req; }

	  private:
		uv_tcp_t m_tcp_handle;

		uv_connect_t m_req; // client only

		int m_fd;

		std::atomic_int m_state;
	};
}; // namespace net
