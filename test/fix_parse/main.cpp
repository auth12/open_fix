#include "include.h"

#include <net.h>

#include <details.h>

#include <fix_parse.h>

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
                           "000\00155=EUR/"
                           "USD\001262=1\001268=2\001269=0\001270=1.2345\001271=100000\00110=080\0018=FIX.4.4\0019="
                           "178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
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

    char nonce[ 32 ];

    for ( int i = 0; i < sizeof( nonce ); i++ ) {
        nonce[ i ] = rand( ) % 256;
    }

    const auto nonce_64 = details::base64_encode( ( unsigned char * )nonce, sizeof( nonce ) );

    auto timestamp =
        std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now( ).time_since_epoch( ) )
            .count( );

    auto raw_data = fmt::format( "{}.{}", timestamp, nonce_64 );

    spdlog::info( "{}", raw_data );

    char buf[ 2048 ];
    fix::fix_writer_t wr( buf, sizeof( buf ) );
    wr.push_header( "FIX.4.4" );
    wr.push_field( fix_spec::tag::MsgType, "A" );
    wr.push_field( fix_spec::tag::SenderCompID, "DERBITICLI" );
    wr.push_field( fix_spec::tag::TargetCompID, "DERIBITSERVER" );
    wr.push_int( fix_spec::tag::MsgSeqNum, 1 );
    wr.push_field( fix_spec::tag::Username, "FnVfw2NS" );
    //wr.push_field( fix_spec::tag::Password, "42fSsqVo1ZDgL5PWXSvcCTZH7D0pCqxpML-mKGSarGY" );
    wr.push_field( fix_spec::tag::RawData, raw_data );
    wr.push_trailer( );

    spdlog::info( "{}", buf );
    spdlog::info( "{:x}", spdlog::to_hex( buf, buf + wr.cur_pos ) );

    return 0;
}
