#include "include.h"

#include <details.h>

#include <fix_parse.h>

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

int main( ) {
    spdlog::set_pattern( "[%t]%+" );

    /*if ( !fix::is_valid_fix( fix_buf ) ) {
        spdlog::error( "invalid fix msg" );
        return 0;
    }

    fix::fix_reader_t rd{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &r : rd ) {
        for ( auto &it : r ) {
            spdlog::info( "{}:{}", it.tag, it.val.as_str( ) );
        }
        spdlog::info( "----" );
    }*/

    spdlog::info( "{}", std::thread::hardware_concurrency( ) );

    return 0;
}
