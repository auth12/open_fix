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
        const char *ptr;

        fix_field_iterator_t( ) = default;
        fix_field_iterator_t( const char *p_ ) : ptr{ p_ } {
            cur.begin = p_;
            ptr = p_;
            auto end = strchr( cur.begin, '\001' );
            if ( !end ) {
                return;
            }

            cur.val.begin = strchr( cur.begin, '=' ) + 1;
            cur.tag = atoi( cur.begin );
            cur.val.end = end;
        }

        reference operator*( ) const { return cur; }
        pointer operator->( ) const { return &cur; }

        const fix_field_iterator_t &operator++( ) {
            next( );
            return *this;
        }

        void next( ) {
            ptr = cur.val.end + 1;
            cur.begin = ptr;
            cur.tag = 0;

            auto end = strchr( cur.begin, '\001' );
            if ( !end ) {
                ptr = cur.begin + 1;
                return;
            }

            cur.val.begin = strchr( cur.begin, '=' ) + 1;
            cur.tag = details::atoi( cur.begin, cur.val.begin - 1 );
            cur.val.end = end;
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
                ptr = end_ + 1;
                return;
            }

            cur.start = ptr;

            const char *b = cur.start + 5;
            while ( b < end_ - 5 ) {
                if ( memcmp( b, "8=FIX", 5 ) == 0 ) {
                    cur.last = b;
                    break;
                }
                b++;
            }
            
            if ( cur.last != b )
                cur.last = end_;
        }

        bool operator!=( const fix_message_iterator_t &o ) { return ptr != o.ptr; };
        bool operator<( const fix_message_iterator_t &o ) { return ptr < o.ptr; };
        bool operator>( const fix_message_iterator_t &o ) { return ptr > o.ptr; };
    };

    struct fix_reader_t {
        using iterator = fix_message_iterator_t;
        const char *begin_, *end_ = nullptr;

        fix_reader_t( const char *b_, const char *e_ ) : begin_{ b_ }, end_{ e_ } {}

        const iterator begin( ) const { return iterator{ begin_, end_ }; }
        const iterator end( ) const { return iterator{ end_, end_ }; }
    };

    struct fix_writer_t {
        char *begin = nullptr;
        size_t len = 0;

        size_t cur_pos = 0;

        fix_writer_t( char *ptr, const size_t len_ ) : begin{ ptr }, len( len_ ) {}

        fix_writer_t &write_header( const std::string_view fix_v ) {
            return *this;
        }

        fix_writer_t &push( const int &tag, const std::string_view val ) {
            if ( cur_pos >= len ) {
                return *this;
            }
            int ret = details::itoa( tag, &begin[ cur_pos ], len - cur_pos );
            cur_pos += ret;
            begin[ cur_pos++ ] = '=';
            memcpy( begin + cur_pos, val.data( ), val.size( ) );
            cur_pos += val.size( );

            begin[ cur_pos++ ] = '\x01';

            return *this;
        }
    };

}; // namespace fix