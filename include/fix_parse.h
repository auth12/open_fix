#pragma once

#include <include.h>
#include <fix_spec.h>

#include <details.h>

#include <spdlog/fmt/chrono.h>

namespace fix {
	struct field_val_t {
		const char *begin, *end = nullptr;

		field_val_t( ) = default;

		field_val_t( const char *b, const char *e ) : begin{ b }, end{ e } {}

		int as_int( ) const { return std::atoi( begin ); }
		auto as_string( ) const { return std::string_view{ begin, ( size_t )( end - begin ) }; }
		double as_double( ) const { return atof( begin ); }
		char as_char( ) const { return begin[ 0 ]; };
	};

	struct fix_field_t {
		int tag;
		field_val_t val;
		const char *begin = nullptr;

		fix_field_t( ) = default;
		fix_field_t( const char *b_ ) : begin{ b_ }, tag{ 0 }, val{ nullptr, nullptr } {}

		bool operator==( const int &t_ ) const { return tag == t_; }
	};

	template < char Delim > struct fix_field_iterator_t {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = fix_field_t;
		using pointer = value_type const *;
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

		void next( ) {
			cur.val.begin = nullptr;
			cur.val.end = nullptr;

			for ( const char *i = cur.begin; *i != '\0'; ++i ) {
				if ( *i == '=' && !cur.val.begin ) {
					cur.val.begin = i + 1;
					continue;
				}

				if ( *i == Delim && !cur.val.end ) {
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

	// single FIX msg
	template < char Delim = '\001' > struct fix_message_t {
		using iterator = fix_field_iterator_t< Delim >;
		const char *begin_, *end_ = nullptr;

		char msg_type = 0;

		fix_message_t( const std::string_view &buf ) : begin_{ buf.data( ) }, end_{ buf.data( ) + buf.size( ) } { };
		template < size_t N > fix_message_t( const char ( &buf )[ N ] ) : begin_{ buf }, end_{ buf + N - 1 } { };
		fix_message_t( const char *b_, const char *e_ ) : begin_{ b_ }, end_{ e_ } { };
		fix_message_t( const char *b_, const size_t len ) : begin_{ b_ }, end_{ b_ + len } { };

		bool validate( ) {
			const ptrdiff_t len = end_ - begin_;
			if ( begin_[ 0 ] != '8' or begin_[ 1 ] != '=' or begin_[ len - 1 ] != '\001' ) {
				return false;
			}

			iterator b{ begin_ };
			if ( b->tag != 8 ) { // BeginString
				return false;
			}
			++b;
			if ( b->tag != 9 ) { // bodylen
				return false;
			}
			++b;
			if ( b->tag != 35 ) { // msgtype
				return false;
			}

			msg_type = b->val.as_char( );
			begin_ = b->val.end + 1;

			return true;
		}

		iterator begin( ) const { return iterator{ begin_ }; }
		iterator end( ) const { return iterator{ end_ }; }
		iterator find( const int &tag ) const { return std::find( begin( ), end( ), tag ); }
	};

	class fix_writer {
	  public:
		fix_writer( char *begin, const size_t len )
			: m_begin{ begin }, m_len{ len }, m_cur{ begin }, m_body_len{ nullptr } {}

		fix_writer &push_header( const std::string_view begin_string ) {
			return push_field( fix_spec::BeginString, begin_string );
		}

		template < typename T > fix_writer &push_field( const int &tag, const T &arg ) {
			m_cur = fmt::format_to( m_cur, "{}={}\001", tag, arg );

			if ( tag == fix_spec::BeginString ) {
				m_body_len = m_cur;
			}
			return *this;
		}

		template < typename Clock >
		fix_writer &push_timestamp( const int &tag, const std::chrono::time_point< Clock > &t ) {
			auto ms = std::chrono::time_point_cast< std::chrono::milliseconds >( t ).time_since_epoch( ).count( ) %
					  std::milli::den;
			m_cur = fmt::format_to( m_cur, "{}={:%Y%m%d-%H:%M:%S}.{:03}\001", tag, fmt::gmtime( Clock::to_time_t( t ) ),
									ms );
			return *this;
		}

		fix_writer &push_trailer( ) {
			const size_t body_len = m_cur - m_body_len;
			const int dig_num = log10( body_len ) + 1;
			const int str_len = dig_num + 3; // 2 (9=) + digits in body_len + SOH

			std::memmove( m_body_len + str_len, m_body_len, m_cur - m_body_len );
			fmt::format_to( m_body_len, "9={}\001", body_len );
			m_cur += str_len;

			int checksum = 0;
			for ( int i = 0; i < ( m_cur - m_begin ); ++i ) {
				checksum += ( uint )m_begin[ i ];
			}

			m_cur = fmt::format_to( m_cur, "10={:03}\001", checksum % 256 );

			return *this;
		}

		size_t size( ) const { return m_cur - m_begin; }

	  private:
		size_t m_len;
		char *m_begin, *m_cur, *m_body_len;
	};

}; // namespace fix