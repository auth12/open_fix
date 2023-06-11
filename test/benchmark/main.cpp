#include "include.h"

#include <fix_parse.h>

#include <benchmark/cppbenchmark.h>

#include <hffix.hpp>

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

struct s_t {
    int id = 0;
    uint8_t a[ 276 ];
};
BENCHMARK( "vector" ) {

    std::vector< std::shared_ptr< s_t > > vec;
    for ( int i = 0; i < 100; i++ ) {
        vec.emplace_back( std::make_shared< s_t >( s_t{ i } ) );
    }
    auto it = std::find_if( vec.begin( ), vec.end( ), []( const std::shared_ptr< s_t > &s ) { return s->id == 50; } );
}
BENCHMARK( "fix_parser iterate" ) {
    std::unordered_map< int, std::shared_ptr< s_t > > vec;
    for ( int i = 0; i < 100; i++ ) {
        vec[ i ] = std::make_shared< s_t >( s_t{ i } );
    }
    auto it = vec.at( 50 );
}


/*


BENCHMARK( "fix_reader iter" ) {
    fix::fix_reader_t rd{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &r : rd ) {
        for ( auto &f : r ) {
        }
    }
}

BENCHMARK( "unordered_map" ) {
    fix::fix_message_t m{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &f : m ) {
    }
}

BENCHMARK( "fix_parser find" ) {
    fix::fix_message_t m{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &f : m ) {
        if ( f.tag == fix_spec::tag::MDEntryPx ) {
            break;
        }
    }
}

BENCHMARK( "fix_reader iter" ) {
    fix::fix_reader_t rd{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &r : rd ) {
        for ( auto &f : r ) {
        }
    }
}

BENCHMARK( "hffix iter" ) {
    hffix::message_reader rd{ fix_buf };
    for ( ; rd.is_complete( ); rd = rd.next_message_reader( ) ) {
        if ( rd.is_valid( ) ) {
            auto begin = rd.begin( );
            if ( rd.find_with_hint( fix_spec::tag::MDEntryPx, begin ) ) {
            }
        }
    }
}*/

BENCHMARK_MAIN( )
