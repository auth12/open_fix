#pragma once

#include <include.h>
#include <fix_spec.h>

#include <details.h>

namespace fix {
    struct field_val_t {
        const char *begin, *end;

        field_val_t( ) : begin{ nullptr }, end{ nullptr } {}

        field_val_t( const char *b, const char *e ) {
            begin = b;
            end = e;
        }

        const int as_int( ) const { return details::atoi( begin, end ); }

        const std::string_view as_str( ) const { return std::string_view{ begin, ( size_t )( end - begin ) }; }

        const float as_float( ) const { return atof( begin ); }

        const char as_char( ) const { return begin[ 0 ]; };
    };

    struct fix_field_t {
        int tag;
        field_val_t val;
        const char *begin = nullptr;

        fix_field_t( ) = default;
        fix_field_t( const char *b_ ) : begin{ b_ } {}

        bool operator!=( const fix_field_t &o ) { return o.begin != begin; };
        bool operator==( const fix_field_t &o ) { return o.begin == begin; };
    };

    struct fix_field_iterator_t {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = fix_field_t;
        using pointer = value_type const *; // or also value_type*
        using reference = value_type const &;

        value_type cur;
        const char *ptr = nullptr;

        fix_field_iterator_t( ) = default;
        fix_field_iterator_t( const char *p_ ) : ptr{ p_ } { next( ); }

        reference operator*( ) const { return cur; }
        pointer operator->( ) const { return &cur; }

        const fix_field_iterator_t &operator++( ) {
            next( );
            return *this;
        }

        void next( ) {
            bool found = false;
            for ( const char *i = ptr; *i; ++i ) {
                if ( *i == '=' ) {
                    cur.val.begin = i + 1;
                    continue;
                }

                if ( *i == '\001' ) {
                    cur.val.end = i;
                    found = true;
                    break;
                }
            }

            if ( !found or !cur.val.begin ) {
                ptr = nullptr;
                return;
            }

            cur.tag = 0;
            cur.begin = ptr;

            cur.tag = details::atoi( cur.begin, cur.val.begin - 1 );

            ptr = cur.val.end + 1;
        }

        bool operator!=( const fix_field_iterator_t &o ) { return o.ptr != ptr; };
        bool operator<( const fix_field_iterator_t &o ) { return o.ptr < ptr; };
        bool operator>( const fix_field_iterator_t &o ) { return o.ptr > ptr; };
    };

    struct tag_equal {
        int tag;
        tag_equal( const int &tag ) : tag( tag ) {}

        bool operator( )( const fix_field_t &v ) const { return v.tag == tag; }
    };

    static bool is_valid_fix( const std::string_view &buf ) {
        return buf.starts_with( "8=" ) && buf.ends_with( '\001' );
    };

    struct fix_message_t {
        using iterator = fix_field_iterator_t;
        const char *start, *last = nullptr;

        fix_message_t( ) = default;
        fix_message_t( const std::string_view &buf ) : start{ buf.data( ) }, last{ buf.data( ) + buf.size( ) } {}

        template < size_t N > fix_message_t( const char ( &buf )[ N ] ) : start{ buf }, last{ buf + N } {}

        fix_message_t( const char *b_, const char *e_ ) : start{ b_ }, last{ e_ } {}

        const iterator begin( ) const { return iterator{ start }; }
        const iterator end( ) const { return iterator{ last }; }
        const iterator find( const int &tag ) const { return std::find_if( begin( ), end( ), tag_equal( tag ) ); }
    };

    struct fix_message_iterator_t {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = fix_message_t;
        using pointer = value_type const *; // or also value_type*
        using reference = value_type const &;

        value_type cur;
        const char *ptr;
        const char *end_;

        fix_message_iterator_t( const char *p_, const char *e_ ) : ptr{ p_ }, end_{ e_ } { next( ); }

        reference operator*( ) const { return cur; }
        pointer operator->( ) const { return &cur; }

        const fix_message_iterator_t &operator++( ) {
            next( );
            return *this;
        }

        void next( ) {
            if ( cur.last )
                ptr = cur.last;

            if ( ptr + 1 > end_ ) {
                ptr = nullptr;
                return;
            }

            const char *end = strstr( ptr + 5, "8=FIX" );

            cur.start = ptr;
            cur.last = end ? end : end_;
        }

        bool operator!=( const fix_message_iterator_t &o ) { return ptr != o.ptr; };
        bool operator<( const fix_message_iterator_t &o ) { return ptr < o.ptr; };
        bool operator>( const fix_message_iterator_t &o ) { return ptr > o.ptr; };
    };

    struct fix_reader_t {
        using iterator = fix_message_iterator_t;
        const char *begin_, *end_ = nullptr;

        fix_reader_t( const char *b_, const char *e_ ) : begin_{ b_ }, end_{ e_ } {}

        fix_reader_t( const std::string_view &buf ) : begin_{ buf.data( ) }, end_{ buf.data( ) + buf.size( ) } {}

        template < size_t N > fix_reader_t( const char ( &buf )[ N ] ) : begin_{ buf }, end_{ buf + N } {}

        const iterator begin( ) const { return iterator{ begin_, end_ }; }
        const iterator end( ) const { return iterator{ end_, end_ }; }
    };

    // no message order checking
    struct fix_writer_t {
        char *begin = nullptr;
        size_t len = 0, cur_pos = 0, body_len_pos = 0;

        fix_writer_t( char *ptr, const size_t len_ ) : begin{ ptr }, len( len_ ) {}

        fix_writer_t &push_header( const std::string_view fix_v ) {
            body_len_pos = push_field( fix_spec::tag::BeginString, fix_v ).cur_pos;
            return *this;
        }

        fix_writer_t &push_trailer( ) {
            if ( cur_pos >= len ) {
                return *this;
            }

            auto len = cur_pos - body_len_pos;
            
            auto len_field = fmt::format( "9={}\001", len );
            
            // shift the buffer
            memmove( begin + body_len_pos + len_field.size( ), begin + body_len_pos, len );
            memcpy( begin + body_len_pos, len_field.data( ), len_field.size( ) );

            cur_pos += len_field.size( );

            uint8_t checksum = std::accumulate( begin, begin + cur_pos, uint8_t{ 0 } );

            memcpy( begin + cur_pos, "10=", 3 );
            cur_pos += 3;
            char *cur = begin + cur_pos;

            cur[ 0 ] = '0' + ( ( checksum / 100 ) % 10 );
            cur[ 1 ] = '0' + ( ( checksum / 10 ) % 10 );
            cur[ 2 ] = '0' + ( checksum % 10 );

            cur[ 3 ] = '\001';

            cur_pos += 4;

            return *this;
        }

        fix_writer_t &push_field( const int &tag, const std::string_view val ) {
            if ( cur_pos >= len ) {
                return *this;
            }
            cur_pos += details::itoa( tag, &begin[ cur_pos ], len - cur_pos );
            begin[ cur_pos++ ] = '=';
            memcpy( begin + cur_pos, val.data( ), val.size( ) );
            cur_pos += val.size( );

            begin[ cur_pos++ ] = '\x01';

            return *this;
        }

        fix_writer_t &push_int( const int &tag, const int val ) {
            if ( cur_pos >= len ) {
                return *this;
            }
            cur_pos += details::itoa( tag, &begin[ cur_pos ], len - cur_pos );
            begin[ cur_pos++ ] = '=';
            cur_pos += details::itoa( tag, &begin[ cur_pos ], len - cur_pos );

            begin[ cur_pos++ ] = '\x01';

            return *this;
        }
    };

}; // namespace fix