#pragma once

#include "include.h"

#include <atomic_queue/atomic_queue.h>

#define LOG_PATTERN "[%t][%X.%e][%n][%^%l%$] %v"

namespace details {
	using log_ptr_t = std::shared_ptr< spdlog::logger >;

	namespace log {
		inline log_ptr_t make_sync( const std::string_view name, bool file = false ) {
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

	template < typename Type, size_t Size, int N > class object_pool {
	  public:
		static constexpr size_t obj_size = Size;

		object_pool( ) {
			m_ptr = std::make_unique< char[] >( Size * N );
			for ( int i = 0; i < N; ++i ) {
				m_pool.push( ( uintptr_t )m_ptr.get( ) + ( Size * i ) );
			}
		}

		// m_pool.pop busy waits but is faster than try_pop when queue isnt empty
		Type *get( ) {
			/*uintptr_t ret = 0;
			if ( !m_pool.try_pop( ret ) ) {
				return ( Type * )ret;
			}*/

			return ( Type * )m_pool.pop( );
		}

		void release( Type *obj ) { m_pool.push( uintptr_t( obj ) ); }

		size_t pool_size( ) { return m_pool.was_size( ); }

	  private:
		atomic_queue::AtomicQueue2< uintptr_t, N > m_pool;
		std::unique_ptr< char[] > m_ptr;
	};
}; // namespace details