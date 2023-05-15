#include "include.h"

#include <net.h>

#include <details.h>

#include <fix_parse.h>
#include <fix_spec.h>


int main( ) {
    std::string s{
        "8=FIX.4.2\0019=186\00135=D\00149=CLIENT1\00156=EXCHANGE1\00134=1\00152=20230514-10:00:00\00111=ORDER123456\001"
        "22=1\00148=XYZ\00155=ABC\00154=1\00160=20230514-10:00:00\00110=178\001"
    };

    fix::fix_message_t m{ s };

    m.for_each_field( [ = ]( const fix::fix_field_t f ) {
        spdlog::info( "{}:{}", f.tag, f.val.as_str( ) );
    } );

    char buf[ 32 ] = { 0 };
    std::string_view val = "SELL";
    
    fix::fix_writer_t wr( &buf[ 0 ], sizeof( buf ) );
    wr.push_field( fix_spec::tag::AllocPrice, 11 );
    wr.push_field( fix_spec::tag::AllocID, 98100 );
    wr.push_field( fix_spec::tag::AdvSide, val );
    
    spdlog::info( "{}", buf );

    return 0;
}