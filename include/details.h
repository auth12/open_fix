#pragma once

#include "include.h"

#include <atomic_queue/atomic_queue.h>

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

	// unsigned atoi
	inline int atou( const char *begin, const char *end ) {
		int ret = 0;
		for ( ; begin < end; ++begin ) {
			ret *= 10;
			ret += static_cast< int >( *begin - '0' );
		}

		return ret;
	}

	inline int itoa( uint num, char *buf, const size_t remaining ) {
		int digits = log10( num ) + 1;
		if ( remaining < digits ) {
			return 0;
		}

		if ( num < 10 ) {
			*buf = '0' + num;
			return 1;
		}

		char *tmp = buf;
		for ( int i = 0; i < digits; ++i, num /= 10 ) {
			*tmp++ = '0' + num % 10;
		}

		std::reverse( buf, tmp );

		return digits;
	}

	template < typename Type, size_t TypeSize, int N > class object_pool {
	  public:
		static constexpr size_t obj_size = TypeSize;

		object_pool( ) {
			for ( int i = 0; i < N; ++i ) {
				m_pool.push( ( uintptr_t )m_allocator.allocate( TypeSize ) );
			}
		}

		// m_pool.pop busy waits but is faster than try_pop when queue isnt empty
		// make sure consumption is fast enough
		Type *get( ) {
			/*uintptr_t ret = 0;
			if ( !m_pool.try_pop( ret ) ) {
				return ( Type * )ret;
			}*/

			return ( Type * )m_pool.pop( );
		}

		void release( Type *obj ) {
			m_pool.push( uintptr_t( obj ) );
			obj = nullptr;
		}

	  private:
		atomic_queue::AtomicQueue2< uintptr_t, N > m_pool;
		std::allocator< Type > m_allocator;
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