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
			for ( int i = 0; i < N; ++i ) {
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

		constexpr size_t obj_size( ) { return Size; }

	  private:
		atomic_queue::AtomicQueue< uintptr_t, N > m_pool;
		tbb::cache_aligned_allocator< T > m_allocator;
	};

	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz"
											"0123456789+/";

	static std::string base64_encode( unsigned char const *bytes_to_encode, unsigned int in_len ) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[ 3 ];
		unsigned char char_array_4[ 4 ];

		while ( in_len-- ) {
			char_array_3[ i++ ] = *( bytes_to_encode++ );
			if ( i == 3 ) {
				char_array_4[ 0 ] = ( char_array_3[ 0 ] & 0xfc ) >> 2;
				char_array_4[ 1 ] = ( ( char_array_3[ 0 ] & 0x03 ) << 4 ) + ( ( char_array_3[ 1 ] & 0xf0 ) >> 4 );
				char_array_4[ 2 ] = ( ( char_array_3[ 1 ] & 0x0f ) << 2 ) + ( ( char_array_3[ 2 ] & 0xc0 ) >> 6 );
				char_array_4[ 3 ] = char_array_3[ 2 ] & 0x3f;

				for ( i = 0; ( i < 4 ); i++ )
					ret += base64_chars[ char_array_4[ i ] ];
				i = 0;
			}
		}

		if ( i ) {
			for ( j = i; j < 3; j++ )
				char_array_3[ j ] = '\0';

			char_array_4[ 0 ] = ( char_array_3[ 0 ] & 0xfc ) >> 2;
			char_array_4[ 1 ] = ( ( char_array_3[ 0 ] & 0x03 ) << 4 ) + ( ( char_array_3[ 1 ] & 0xf0 ) >> 4 );
			char_array_4[ 2 ] = ( ( char_array_3[ 1 ] & 0x0f ) << 2 ) + ( ( char_array_3[ 2 ] & 0xc0 ) >> 6 );
			char_array_4[ 3 ] = char_array_3[ 2 ] & 0x3f;

			for ( j = 0; ( j < i + 1 ); j++ )
				ret += base64_chars[ char_array_4[ j ] ];

			while ( ( i++ < 3 ) )
				ret += '=';
		}

		return ret;
	}
}; // namespace details