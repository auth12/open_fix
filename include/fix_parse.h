#pragma once

#include <include.h>
#include <fix_spec.h>

#include <details.h>

namespace fix {
    struct field_val_t {
        const char *begin, *end;
        size_t len = 0;

        field_val_t( ) : begin{ nullptr }, end{ nullptr } {}

        void set( const std::string_view val ) {
            begin = val.data( );
            end = val.data( ) + val.size( );

            len = end - begin;
        }

        int as_int( ) const { return details::atoi( begin, end ); }

        std::string_view as_str( ) const { return std::string_view{ begin, len }; }
    };

    struct fix_field_t {
        int tag = -1;
        field_val_t val;

        fix_field_t( const fix_field_t & ) = delete;
        fix_field_t( fix_field_t && ) = default;
        fix_field_t &operator=( const fix_field_t & ) = delete;
        fix_field_t &operator=( fix_field_t && ) = default;

        fix_field_t( ) = default;
        fix_field_t( const char *begin, const char *end ) { fill( { begin, ( size_t )( end - begin ) } ); }

        bool fill( const std::string_view field ) {
            size_t delim_pos = 0;
            for ( int i = 0; i < field.size( ); ++i ) {
                if ( field[ i ] == '=' ) {
                    delim_pos = i;
                    break;
                }
            }

            if ( delim_pos == 0 ) {
                return false;
            }

            tag = details::atoi( &field[ 0 ], &field[ delim_pos ] );
            val.set( { &field[ delim_pos + 1 ], field.size( ) - delim_pos + 1 } );
            return true;
        }
    };

    inline void *apply_field_value( char *val_begin, const char *val, const size_t val_len ) {
        return memcpy( val_begin, val, val_len );
    }

    struct fix_message_t {
        char const *start, *end;

        fix_message_t( std::string_view buf ) {
            start = buf.data( );
            end = buf.data( ) + buf.size( );
        }

        template < typename Fn > void for_each_field( const Fn &&f_ ) {
            const char *last = start;
            for ( auto i = start; i < end; ++i ) {
                if ( *i == '\001' ) {
                    f_( fix_field_t( last, i - 1 ) );
                    last = i + 1;
                }
            }

            if ( last < end )
                f_( fix_field_t( last, end ) );
        }
    };

    struct fix_writer_t {
        char *begin = nullptr;
        size_t len = 0;

        size_t cur_pos = 0;

        fix_writer_t( char *ptr, const size_t len_ ) : begin{ ptr }, len( len_ ) {}

        template < typename val_type > void push_field( const int tag, const val_type &val ) {
            if ( cur_pos >= len ) {
                return;
            }

            int ret = details::itoa( tag, &begin[ cur_pos ], len - cur_pos );
            cur_pos += ret;
            begin[ cur_pos++ ] = '=';

            if constexpr ( std::is_arithmetic< val_type >::value ) {
                ret = details::itoa( val, &begin[ cur_pos ], len - cur_pos );
                cur_pos += ret;
            }

            if constexpr ( std::is_same_v< val_type, std::string_view > ) {
                memcpy( begin + cur_pos, val.data( ), val.size( ) );
                cur_pos += val.size( );
            }

            // delim
            begin[ cur_pos++ ] = '\x01';
        }
    };

}; // namespace fix