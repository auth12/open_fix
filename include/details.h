#pragma once

#include "include.h"

#include <atomic_queue/atomic_queue.h>

#include <tbb/cache_aligned_allocator.h>

namespace details {
	using log_ptr_t = std::shared_ptr< spdlog::logger >;

	namespace log {

		inline log_ptr_t make_sync( const std::string_view &name, bool file = false ) {
			std::vector< spdlog::sink_ptr > sinks;
			sinks.emplace_back( std::make_shared< spdlog::sinks::stdout_color_sink_mt >( ) );
			if ( file ) {
				sinks.emplace_back(
					std::make_shared< spdlog::sinks::rotating_file_sink_mt >( name.data( ), 1024 * 1024 * 5, 10 ) );
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

	template < typename T, size_t Size, int N > class object_pool {
	  public:
        object_pool( ) {
            for( int i = 0; i < N; ++i ) {
                m_pool.try_push( ( uintptr_t )m_allocator.allocate( Size ) );
            }
        }

		T *get( ) {
			uintptr_t ret = 0;
			if ( !m_pool.try_pop( ret ) ) {
				return ( T * )ret;
			}

			return ( T * )ret;
		}

		void release( T *obj ) { m_pool.try_push( uintptr_t( obj ) ); }

        constexpr size_t obj_size( ) {
            return Size;
        }

	  private:
		atomic_queue::AtomicQueue< uintptr_t, N > m_pool;
        tbb::cache_aligned_allocator< T > m_allocator;
	};
}; // namespace details