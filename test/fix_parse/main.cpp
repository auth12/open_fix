#include "include.h"

#include <net.h>

#include <details.h>

#include <fix_parse.h>

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\0018=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=89\00110=080\001";

int main( ) {
    if ( !fix::is_valid_fix( fix_buf ) ) {
        spdlog::error( "invalid fix msg" );
        return 0;
    }

    fix::fix_reader_t rd{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &r : rd ) {
        for ( auto &it : r ) {
            if ( it.tag == fix_spec::tag::MDEntryPx ) {
                spdlog::info( "price entry: {}", it.val.as_float( ) );
            }
            spdlog::info( "{}:{}", it.tag, it.val.as_str( ) );
        }
        spdlog::info( "----" );
    }

    char buf[ 2048 ];
    memset( buf, 0, 2048 );
    fix::fix_writer_t wr( buf, 2048 );
    wr.push( 8, "FIX.4.4" ).push( 270, "1.897" );

    spdlog::info( "{}", buf );

    return 0;
}
