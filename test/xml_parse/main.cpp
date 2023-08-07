#include "include.h"

#include <fix_parse.h>

#include "pugixml.hpp"

struct field_t {
	int id = -1;
	bool required = false;
};

struct header_t {
	std::vector< field_t > fields;
};

struct trailer_t {
	std::vector< field_t > fields;
};

struct message_t {
	std::vector< field_t > fields;
};

class fix_dictionary : public pugi::xml_document {
  private:
	int m_fix_maj, m_fix_min;

	std::vector< field_t > m_header;
	std::vector< field_t > m_trailer;

	std::unordered_map< std::string, std::vector< field_t > > m_message_fields;
	std::unordered_map< std::string, std::string > m_message_desc;

	std::unordered_map< int, std::string > m_field_dict;
	std::unordered_map< std::string, int > m_field_desc; // ????

  public:
	auto &messages( ) { return m_message_fields; };
	auto &field_desc( ) { return m_field_desc; }
	auto &field_dict( ) { return m_field_dict; }

	auto begin_str( ) { return fmt::format( "FIX.{}.{}", m_fix_maj, m_fix_min ); }

	bool load( const std::string_view dict ) {
		auto ret = load_file( dict.data( ) );
		if ( !ret ) {
			return false;
		}

		auto fix_data = child( "fix" );
		if ( !fix_data ) {
			return false;
		}

		m_fix_maj = fix_data.attribute( "major" ).as_int( );
		m_fix_min = fix_data.attribute( "minor" ).as_int( );

		auto fields = fix_data.child( "fields" );
		for ( auto &f : fields.children( ) ) {
			auto name = f.attribute( "name" ).as_string( );
			int id = f.attribute( "number" ).as_int( );

			m_field_dict[ id ] = name;
			m_field_desc[ name ] = id;
		}

		auto header = fix_data.child( "header" );
		for ( auto &c : header.children( ) ) {
			auto name = c.attribute( "name" ).as_string( );
			auto req = c.attribute( "required" ).as_string( );

			m_header.emplace_back( field_t{ m_field_desc[ name ], strcmp( req, "Y" ) > 0 } );
		}

		auto messages = fix_data.child( "messages" );
		for ( auto &m : messages.children( ) ) {
			auto msg_name = m.attribute( "name" ).as_string( );
			auto type = m.attribute( "msgtype" ).as_string( );

			m_message_desc[ msg_name ] = type;

			for ( auto &f : m.children( ) ) {
				auto name = f.attribute( "name" ).as_string( );
				auto req = f.attribute( "required" ).as_string( );

				m_message_fields[ msg_name ].emplace_back( field_t{ m_field_desc[ name ], strcmp( req, "Y" ) == 0 } );
			}
		}

		return true;
	}
};

int main( ) {
	fix_dictionary dic{ };
	if ( !dic.load( "fix.xml" ) ) {
		spdlog::error( "failed to load fix dictionary" );

		return 0;
	}
	auto field_dic = dic.field_dict( );

	

	return 0;
};