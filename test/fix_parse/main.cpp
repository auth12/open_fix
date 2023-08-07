#include "include.h"

#include <details.h>

#include <fix_parse.h>

#include <ranges>

#include <string_view>

constexpr char fix_buf[] =
	"8=FIX.4.4\0019=149\00135=A\00198=0\001553=FnVfw2NS554=tpBLRqLV7vkp5lgWXa9st6OV+l5wIV00R4OPjVFS9o0=\00196="
	"1686700968042.HpiN3uFKdvLOxHJEWjO0pPk+jvnBWNg7RPXYvpIym4I=\00110=185\001";

int main( ) {
	spdlog::set_pattern( "[%t]%+" );

	fix::fix_message_t msg{ fix_buf };
	msg.init( );
	if( !msg.valid ) {
		spdlog::error( "invalid msg" );
		return 0;
	}

	spdlog::info( "Checksum: {}, msg type: {}", msg.checksum, msg.type );
	for ( auto &f : msg ) {
		spdlog::info( "{}->{}", f.tag, f.val.as_string( ) );
	}

	return 0;
}
