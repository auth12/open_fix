#pragma once

#include "include.h"

#include <unistd.h>

namespace net {
	class tcp_session {
	  protected:
		int m_fd;

	  public:
		tcp_session( ) : m_fd{ -1 } {}
		tcp_session( const int fd ) : m_fd{ fd } {}

		// write data to file descriptor
		int write( const void *data, const size_t len ) const { return send( m_fd, data, len, 0 ); }
		int write( const std::string_view buf ) const { return write( buf.data( ), buf.size( ) ); }

		// read data from file descriptor
		int read( void *data, const size_t len ) const { return recv( m_fd, data, len, 0 ); }

		void close( ) {
			::close( m_fd );
			m_fd = -1;
		}

		void set_fd( const int fd ) { m_fd = fd; } // remove me
		inline int fd( ) const { return m_fd; }

		bool operator==( const tcp_session &o ) { return m_fd == o.fd( ); }
		bool operator==( const int fd ) { return m_fd == fd; }
		operator int( ) const { return m_fd; }
	};
	
	static int write( const int fd, const void *data, size_t len ) { return send( fd, data, len, 0 ); }
	static int write( const int fd, const std::string_view buf ) { return write( fd, buf.data( ), buf.size( ) ); }
	static int read( const int fd, void *data, size_t len ) { return recv( fd, data, len, 0 ); }
}; // namespace net
