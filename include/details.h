#pragma once

#include "include.h"

namespace details {
    using log_ptr_t = std::shared_ptr< spdlog::logger >;

    namespace log {
        inline log_ptr_t make_async( std::string_view name, std::shared_ptr< spdlog::details::thread_pool > &tp,
                                     int threads = 1, int buflen = 1024 ) {
            tp = std::make_shared< spdlog::details::thread_pool >( buflen, threads );

            std::string logfile_name{ name };
            logfile_name.append( ".log" );

            std::vector< spdlog::sink_ptr > sinks;
            sinks.emplace_back( std::make_shared< spdlog::sinks::stdout_color_sink_mt >( ) );
            sinks.emplace_back(
                std::make_shared< spdlog::sinks::rotating_file_sink_mt >( logfile_name, 1024 * 1024 * 5, 10 ) );

            return std::make_shared< spdlog::async_logger >( name.data( ), sinks.begin( ), sinks.end( ), tp,
                                                             spdlog::async_overflow_policy::block );
        }

        inline log_ptr_t make_sync( std::string_view name ) {
            std::string logfile_name{ name };
            logfile_name.append( ".log" );

            std::vector< spdlog::sink_ptr > sinks;
            sinks.emplace_back( std::make_shared< spdlog::sinks::stdout_color_sink_mt >( ) );
            sinks.emplace_back(
                std::make_shared< spdlog::sinks::rotating_file_sink_mt >( logfile_name, 1024 * 1024 * 5, 10 ) );

            return std::make_shared< spdlog::logger >( name.data( ), sinks.begin( ), sinks.end( ) );
        }
    }; // namespace log

    inline int atoi( const char *begin, const char *end, const int base = 10 ) {
        int ret = 0;
        bool negative = false;
        if ( !begin or !end ) {
            return std::numeric_limits< int >::max( );
        }

        if ( *begin == '-' ) {
            negative = true;
            ++begin;
        }

        for ( ; begin < end; ++begin ) {
            ret *= base;
            ret += static_cast< int >( *begin - '0' );
        }

        return negative ? -ret : ret;
    }

    // returns bytes written
    // -1 when the buffer is not enough
    template < typename Num_t > int itoa( Num_t num, char *buf, const size_t len ) {
        bool negative = false;
        if ( num < 0 ) {
            negative = true;
            num = -num;
        }

        if( len < sizeof( num ) + 1 ) {
            return -1;
        }

        int ret = 0;
        auto ptr = buf;
        for ( ; num; num /= 10 ) {
            if ( ptr >= buf + len ) {
                return -1;
            }
            *ptr++ = '0' + ( num % 10 );
            ++ret;
        }

        if ( negative ) {
            if ( ptr >= buf + len ) {
                return -1;
            }
            *ptr++ = '-';
            ret++;
        }

        std::reverse( buf, ptr );

        return ret;
    }

    struct buf_t {
        char *ptr;
        size_t len;

        buf_t( ) : ptr{ nullptr }, len{ 0 } {}
    };

    template < typename T > using lockless_queue_t = tbb::concurrent_queue< T >;
    // basically a buffer ring
    template < int S, int N > struct bufpool_t {
        tbb::scalable_allocator< char > allocator;
        lockless_queue_t< char * > q;

        static const int buf_size = S;

        bufpool_t( ) {

            for ( int i = 0; i < N; ++i ) {
                q.push( allocator.allocate( S ) );
            }
        }

        char *get( ) {
            char *ret = nullptr;
            if ( !q.try_pop( ret ) ) {
                return { };
            }

            return ret;
        }

        void release( char *buf ) { q.push( buf ); }
    };

    static const unsigned char base64_table[ 65 ] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    static std::string base64_encode( const unsigned char *src, size_t len ) {
        unsigned char *out, *pos;
        const unsigned char *end, *in;

        size_t olen;

        olen = 4 * ( ( len + 2 ) / 3 ); /* 3-byte blocks to 4-byte */

        if ( olen < len )
            return { };

        std::string outStr;
        outStr.resize( olen );
        out = ( unsigned char * )&outStr[ 0 ];

        end = src + len;
        in = src;
        pos = out;
        while ( end - in >= 3 ) {
            *pos++ = base64_table[ in[ 0 ] >> 2 ];
            *pos++ = base64_table[ ( ( in[ 0 ] & 0x03 ) << 4 ) | ( in[ 1 ] >> 4 ) ];
            *pos++ = base64_table[ ( ( in[ 1 ] & 0x0f ) << 2 ) | ( in[ 2 ] >> 6 ) ];
            *pos++ = base64_table[ in[ 2 ] & 0x3f ];
            in += 3;
        }

        if ( end - in ) {
            *pos++ = base64_table[ in[ 0 ] >> 2 ];
            if ( end - in == 1 ) {
                *pos++ = base64_table[ ( in[ 0 ] & 0x03 ) << 4 ];
                *pos++ = '=';
            } else {
                *pos++ = base64_table[ ( ( in[ 0 ] & 0x03 ) << 4 ) | ( in[ 1 ] >> 4 ) ];
                *pos++ = base64_table[ ( in[ 1 ] & 0x0f ) << 2 ];
            }
            *pos++ = '=';
        }

        return outStr;
    }
}; // namespace details