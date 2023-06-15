#pragma once

#include <include.h>
#include <fix_spec.h>

#include <details.h>

namespace fix {
	struct field_val_t {
		const char *begin, *end = nullptr;

		field_val_t( ) = default;

		field_val_t( const char *b, const char *e ) : begin{ b }, end{ e } {}

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

		bool operator==( const int &t_ ) const { return tag == t_; }
	};

	struct fix_field_iterator_t {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = fix_field_t;
		using pointer = value_type const *; // or also value_type*
		using reference = value_type const &;

		value_type cur;

		fix_field_iterator_t( ) = default;
		fix_field_iterator_t( const char *p_ ) : cur{ p_ } { next( ); }

		reference operator*( ) const { return cur; }
		pointer operator->( ) const { return &cur; }

		const fix_field_iterator_t &operator++( ) {
			next( );
			return *this;
		}

		const fix_field_iterator_t &operator+( const int &o ) {
			for ( int i = 0; i < o; ++i )
				next( );
			return *this;
		}

		void next( ) {
			cur.val.begin = nullptr;
			cur.val.end = nullptr;

			for ( const char *i = cur.begin; *i; ++i ) {
				if ( *i == '=' && !cur.val.begin ) {
					cur.val.begin = i + 1;
					continue;
				}

				if ( *i == '\001' && !cur.val.end ) {
					cur.val.end = i;
					break;
				}
			}

			if ( !cur.val.end or !cur.val.begin ) {
				cur.begin = nullptr;
				return;
			}

			cur.tag = details::atoi( cur.begin, cur.val.begin - 1 );
			cur.begin = cur.val.end + 1;
		}

		bool operator!=( const fix_field_iterator_t &o ) const { return o.cur.begin != cur.begin; };
		bool operator==( const fix_field_iterator_t &o ) const { return o.cur.begin == cur.begin; };
		bool operator<( const fix_field_iterator_t &o ) const { return o.cur.begin < cur.begin; };
		bool operator>( const fix_field_iterator_t &o ) const { return o.cur.begin > cur.begin; };
	};

	static bool is_valid_fix( const std::string_view &buf ) {
		return buf.starts_with( "8=" ) && buf.ends_with( '\001' );
	};

	// single FIX msg
	struct fix_message_t {
		using iterator = fix_field_iterator_t;
		const char *begin_, *end_ = nullptr;

		fix_message_t( ) = default;
		fix_message_t( const std::string_view &buf ) : begin_{ buf.data( ) }, end_{ buf.data( ) + buf.size( ) } {}

		template < size_t N > fix_message_t( const char ( &buf )[ N ] ) : begin_{ buf }, end_{ buf + N } {}

		fix_message_t( const char *b_, const char *e_ ) : begin_{ b_ }, end_{ e_ } {}
		fix_message_t( const char *b_, const size_t len ) : begin_{ b_ }, end_{ b_ + len } {}

		iterator begin( ) const { return iterator{ begin_ }; }
		iterator end( ) const { return iterator{ end_ }; }
		iterator find( const int &tag ) const { return std::find( begin( ), end( ), tag ); }
	};

	// multiple msg iterator
	struct fix_message_iterator_t {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = fix_message_t;
		using pointer = value_type const *; // or also value_type*
		using reference = value_type const &;

		value_type cur;
		const char *end_;

		fix_message_iterator_t( const char *p_, const char *e_ ) : cur{ p_, nullptr }, end_{ e_ } { next( ); }

		reference operator*( ) const { return cur; }
		pointer operator->( ) const { return &cur; }

		const fix_message_iterator_t &operator++( ) {
			next( );
			return *this;
		}

		void next( ) {
			if ( cur.end_ )
				cur.begin_ = cur.end_;

			if ( cur.begin_ + 1 > end_ ) {
				cur.end_ = nullptr;
				return;
			}

			const char *end = strstr( cur.begin_ + 5, "8=FIX" );

			cur.end_ = end ? end : end_;
		}

		bool operator!=( const fix_message_iterator_t &o ) { return cur.begin_ != o.cur.begin_; };
		bool operator<( const fix_message_iterator_t &o ) { return cur.begin_ < o.cur.begin_; };
		bool operator>( const fix_message_iterator_t &o ) { return cur.begin_ > o.cur.begin_; };
	};

	struct fix_reader_t {
		using iterator = fix_message_iterator_t;
		const char *begin_, *end_ = nullptr;

		fix_reader_t( const char *b_, const char *e_ ) : begin_{ b_ }, end_{ e_ } {}

		fix_reader_t( const std::string_view &buf ) : begin_{ buf.data( ) }, end_{ buf.data( ) + buf.size( ) } {}

		template < size_t N > fix_reader_t( const char ( &buf )[ N ] ) : begin_{ buf }, end_{ buf + N } {}

		const iterator begin( ) const { return iterator{ begin_, end_ }; }
		const iterator end( ) const { return iterator{ end_, nullptr }; }
	};

	class fix_writer {
	  public:
		fix_writer( char *begin, const size_t len )
			: m_buf{ begin }, m_len{ len }, m_cur_pos{ 0 }, m_body_len_pos{ 0 } {}

		fix_writer &push_header( const int fix_maj, const int fix_min ) {
			return push_str( fix_spec::BeginString, fmt::format( "FIX.{}.{}", fix_maj, fix_min ) );
		}

		fix_writer &push_char( const int &tag, const char c ) {
			m_cur_pos += details::itoa( tag, m_buf + m_cur_pos, m_len - m_cur_pos );
			m_buf[ m_cur_pos++ ] = '=';
			m_buf[ m_cur_pos++ ] = c;
			m_buf[ m_cur_pos++ ] = '\001';

			return *this;
		}

		template < typename T > fix_writer &push_int( const int &tag, const T &num ) {
			if constexpr ( std::is_arithmetic_v< T > ) {
				m_cur_pos += details::itoa( tag, m_buf + m_cur_pos, m_len - m_cur_pos );
				m_buf[ m_cur_pos++ ] = '=';
				m_cur_pos += details::itoa( num, m_buf + m_cur_pos, m_len - m_cur_pos );
				m_buf[ m_cur_pos++ ] = '\001';
				return *this;
			}

			return *this;
		}

		fix_writer &push_str( const int &tag, const std::string_view val ) {
			m_cur_pos += details::itoa( tag, m_buf + m_cur_pos, m_len - m_cur_pos );
			m_buf[ m_cur_pos++ ] = '=';
			memcpy( m_buf + m_cur_pos, val.data( ), val.size( ) );
			m_cur_pos += val.size( );
			m_buf[ m_cur_pos++ ] = '\x01';

			if ( tag == fix_spec::BeginString && !m_body_len_pos ) {
				m_body_len_pos = m_cur_pos;
			}

			return *this;
		}

		fix_writer &push_trailer( ) {
			const size_t body_len = m_cur_pos - m_body_len_pos;
			auto len_str = fmt::format( "9={}\001", body_len );

			memmove( m_buf + m_body_len_pos + len_str.size( ), m_buf + m_body_len_pos, body_len );
			memcpy( m_buf + m_body_len_pos, len_str.data( ), len_str.size( ) );

			m_cur_pos += len_str.size( );

			long idx;
			unsigned int cks;

			for ( idx = 0L, cks = 0; idx < m_cur_pos; cks += ( unsigned int )m_buf[ idx++ ] );

			auto checksum_field = fmt::format( "10={:03}\001", cks % 256 );

			memcpy( m_buf + m_cur_pos, checksum_field.data( ), checksum_field.size( ) );

			m_cur_pos += checksum_field.size( );

			return *this;
		}

		auto size( ) const { return m_cur_pos; }

	  private:
		size_t m_cur_pos, m_body_len_pos, m_len;
		char *m_buf;
	};

}; // namespace fix