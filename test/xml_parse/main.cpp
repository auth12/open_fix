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

int main( ) {
	pugi::xml_document doc;
	if ( !doc.load_file( "fix.xml" ) ) {
		spdlog::error( "failed to load file" );
		return 0;
	}

	auto fix = doc.child( "fix" );
	if ( !fix ) {
		spdlog::error( "xml document doesnt contain fix specifications" );
		return 0;
	}

	auto header = fix.child( "header" );
	auto trailer = fix.child( "trailer" );
	auto fields = fix.child( "fields" );

	auto messages = fix.child( "messages" );

	std::unordered_map< std::string, int > field_map;

	header_t hdr;
	trailer_t tr;

	for ( auto &f : fields ) {
		auto name = f.attribute( "name" ).as_string( );
		auto num = f.attribute( "number" ).as_int( );

		field_map[ name ] = num;
	}

	for ( auto &e : header.children( ) ) {
		auto name = e.attribute( "name" ).as_string( );
		auto required = e.attribute( "required" ).as_string( );

		hdr.fields.emplace_back( field_t{ field_map[ name ], strcmp( required, "Y" ) == 0 } );

		spdlog::info( "{}->{}", name, required );
	}

	for ( auto &e : trailer.children( ) ) {
		auto name = e.attribute( "name" ).as_string( );
		auto required = e.attribute( "required" ).as_string( );

		tr.fields.emplace_back( field_t{ field_map[ name ], strcmp( required, "Y" ) > 0 } );
	}

	std::unordered_map< std::string, std::string > msg_type_desc;
	std::unordered_map< std::string, std::vector< int > > msg_types;

	for ( auto &m : messages.children( ) ) {
		auto name = m.attribute( "name" ).as_string( );
		auto id = m.attribute( "msgtype" ).as_string( );
		spdlog::info( "{}:{}", name, id );

		msg_type_desc[ name ] = id;

		auto &entries = msg_types[ name ];

		for ( auto &f : m.children( ) ) {
			auto name = f.attribute( "name" ).as_string( );
			auto required = f.attribute( "required" ).as_string( );

			entries.emplace_back( field_map[ name ] );
			spdlog::info( "=> {}->{}", name, required );
		}
	}

	spdlog::info( "header fields:" );
	for ( auto &f : hdr.fields ) {
		spdlog::info( "{}->{}", f.id, f.required );
	}

	spdlog::info( "logon" );

	for( auto &f : msg_types[ "Logon" ] ) {
		spdlog::info( "{}", f );
	}

	return 0;
};