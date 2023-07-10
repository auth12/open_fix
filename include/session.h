#pragma once

#include "include.h"

#include <unistd.h>

namespace net {
	class tcp_session {
	  private:
		uv_poll_t m_poll_handle;
		int m_fd;

	  public:
		tcp_session( ) : m_fd{ -1 } {}
		tcp_session( uv_loop_t *loop, const int &fd ) : m_fd{ fd } { poll_init( loop ); }

		int poll_init( uv_loop_t *loop ) { return uv_poll_init_socket( loop, &m_poll_handle, m_fd ); }
		int poll_stop( ) { return uv_poll_stop( &m_poll_handle ); }
		template < typename Fn > int poll_start( const Fn &&on_poll, const int events ) {
			return uv_poll_start( &m_poll_handle, events, on_poll );
		}

		int write( const void *data, const size_t &len ) const { return send( m_fd, data, len, 0 ); }
		int write( const std::string_view &buf ) const { return write( buf.data( ), buf.size( ) ); }
		int read( void *data, const size_t len ) const { return recv( m_fd, data, len, 0 ); }

		void reset( ) {
			::close( m_fd );
			m_fd = -1;
		}

		void set_fd( const int &fd ) { m_fd = fd; }
		inline int fd( ) const { return m_fd; }
		uv_poll_t *poll_handle( ) { return &m_poll_handle; }

		bool operator==( const int &o ) const { return m_fd == o; }
	};
}; // namespace net
