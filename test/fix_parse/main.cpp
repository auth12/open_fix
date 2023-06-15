#include "include.h"

#include <details.h>

#include <fix_parse.h>

constexpr char fix_buf[] = "8=FIX.4.4\0019=000134\00135=A\00198=0\001553=FnVfw2NS554=tpBLRqLV7vkp5lgWXa9st6OV+l5wIV00R4OPjVFS9o0=\00196=1686700968042.HpiN3uFKdvLOxHJEWjO0pPk+jvnBWNg7RPXYvpIym4I=\00110=185\001";

int main( ) {
    spdlog::set_pattern( "[%t]%+" );

    if ( !fix::is_valid_fix( fix_buf ) ) {
        spdlog::error( "invalid fix msg" );
        return 0;
    }

    fix::fix_reader_t rd{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &r : rd ) {
        for ( auto &it : r ) {
            spdlog::info( "{}:{}", it.tag, it.val.as_str( ) );

            if( it.tag == fix_spec::tag::BodyLength ) {
                std::string_view checksum{ it.val.end + 1, ( size_t )it.val.as_int( ) };
                spdlog::info( checksum );
                //if( it.val.end + it.val.as_int( ) + 1  )
            }
        }
        spdlog::info( "----" );
    }



    spdlog::info( "{}", std::thread::hardware_concurrency( ) );

    return 0;
}
