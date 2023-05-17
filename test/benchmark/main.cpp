#include "include.h"

#include <fix_parse.h>

#include <benchmark/cppbenchmark.h>

#include <hffix.hpp>

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\001";

BENCHMARK( "fix_parser iterate" ) {
    fix::fix_message_t m{ fix_buf, fix_buf + sizeof( fix_buf ) };
    for ( auto &f : m ) {
    }
}


BENCHMARK( "fix_parser find" ) {
    fix::fix_message_t m{ fix_buf, fix_buf + sizeof( fix_buf ) };
    auto it = m.find( 270 );
}

/*
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
