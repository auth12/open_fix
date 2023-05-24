#pragma once

#include "include.h"

#include <atomic_queue/atomic_queue.h>

#include <tbb/cache_aligned_allocator.h>

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

        inline log_ptr_t make_sync( const std::string_view &name, bool file = false ) {
            std::string logfile_name{ name };
            logfile_name.append( ".log" );

            std::vector< spdlog::sink_ptr > sinks;
            sinks.emplace_back( std::make_shared< spdlog::sinks::stdout_color_sink_mt >( ) );
            if ( file ) {
                sinks.emplace_back(
                    std::make_shared< spdlog::sinks::rotating_file_sink_mt >( logfile_name, 1024 * 1024 * 5, 10 ) );
            }

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

        if ( len < sizeof( num ) + 1 ) {
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

    template < typename T, const size_t N > using lockless_queue_t = atomic_queue::AtomicQueue2< T, N >;
    // basically a buffer ring
    template < int BufSize, int Elements > struct bufpool_t {
        tbb::cache_aligned_allocator< char > allocator;
        lockless_queue_t< char *, Elements > queue;

        static const int buf_size = BufSize;

        bufpool_t( ) {
            for ( int i = 0; i < Elements; ++i ) {
                const auto ptr = allocator.allocate( BufSize );
                memset( ptr, 0, BufSize );
                queue.push( ptr );
            }
        }

        char *get( ) {
            char *ret = nullptr;
            if ( !queue.try_pop( ret ) ) {
                return ret;
            }

            return ret;
        }

        void release( char *buf ) { queue.push( buf ); }
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