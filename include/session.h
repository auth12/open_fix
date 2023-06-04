#pragma once

#include "include.h"

namespace net {
    enum session_state : int { Offline = 0, Idle };

    class tcp_session {
      public:
        tcp_session( ) : m_net_ctx{ -1 }, m_state{ Offline } {}

        int poll_init( uv_loop_t *loop ) {
            // assert( m_net_ctx.fd != -1 );
            return uv_poll_init_socket( loop, &m_poll_handle, m_net_ctx.fd );
        }

        template < typename Fn > int poll_start( const Fn &&on_poll, const int events ) {
            return uv_poll_start( &m_poll_handle, events, on_poll );
        }

        int write( const void *data, const size_t len ) {
            return mbedtls_net_send( &m_net_ctx, ( unsigned char * )data, len );
        }

        int write( const std::string_view buf ) {
            return write( buf.data( ), buf.size( ) );
        }

        int read( void *dst, const size_t len ) {
            return mbedtls_net_recv( &m_net_ctx, ( unsigned char * )dst, len );
        }

        void close( ) {
            mbedtls_net_close( &m_net_ctx );
            mbedtls_net_free( &m_net_ctx );

            m_state = Offline;
        }

        void poll_stop( ) { uv_poll_stop( &m_poll_handle ); }

        int fd( ) const { return m_net_ctx.fd; }

        mbedtls_net_context *net_ctx( ) { return &m_net_ctx; }

        uv_poll_t *poll_handle( ) { return &m_poll_handle; }

        void set_ip( const std::string_view ip ) { m_ip = ip; }

        auto &ip( ) { return m_ip; }
        

        void set_state( const int &state ) {
            m_state = state;
        }

        auto &state( ) { return m_state; }

      private:
        mbedtls_net_context m_net_ctx;
        uv_poll_t m_poll_handle;

        std::string m_ip;

        std::atomic_int m_state;
    };
}; // namespace net
