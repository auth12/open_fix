#include "include.h"

#include "fix_client.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <config.h>

#include <sched.h>
#include <pthread.h>

void consumer( fix::fix_client &cli ) {
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );
	auto &queue = cli.msg_queue( );

	while ( 1 ) {
		auto msg = queue.pop( );
		if ( !msg->buf ) {
			log->critical( "invalid buffer in queue." );
			continue;
		}

		std::string_view buf{ msg->buf, msg->len };

		log->info( buf );

		fix::fix_message_t rd{ buf };
		auto cur = rd.begin( );
		if ( cur->tag != fix_spec::BeginString ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::BodyLength ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		++cur;
		if ( cur->tag != fix_spec::MsgType ) {
			ctx->bufpool.release( msg->buf );
			continue;
		}

		// add checksum field check

		log->info( "fix message, type: {}, len: {}, fd: {}", cur->val.as_str( ), msg->len, msg->session->fd( ) );

		++cur;

		for ( ; cur < rd.end( ); ++cur ) {
			if ( cur->tag == fix_spec::AvgPx ) {
				log->info( "got px update: {}", cur->val.as_float( ) );
			}
		}

		ctx->bufpool.release( msg->buf );
	}
}

void con_cb( net::tcp_session *session ) {

	// fix::fix_writer_t wr{ out_buf, sizeof( out_buf ) };

	const std::string user{ "FnVfw2NS" };
	const std::string secret{ "42fSsqVo1ZDgL5PWXSvcCTZH7D0pCqxpML-mKGSarGY" };

	// nonce
	unsigned char nonce[ 32 ] = { 0 };
	srand( time( 0 ) );
	for ( int i = 0; i < sizeof( nonce ); ++i ) {
		nonce[ i ] = rand( ) % 255;
	}

	const auto nonce64 = details::base64_encode( nonce, sizeof( nonce ) );

	spdlog::info( "nonce64: {}", nonce64 );

	const auto timestamp =
		std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now( ).time_since_epoch( ) )
			.count( );

	auto raw_data = fmt::format( "{}.{}", timestamp, nonce64 );
	auto raw_data_and_secret = raw_data + secret;

	spdlog::info( "raw data: {}, raw data + secret: {}", raw_data, raw_data_and_secret );

	unsigned char hash[ SHA256_DIGEST_LENGTH ];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, raw_data_and_secret.c_str(), raw_data_and_secret.size());
    SHA256_Final(hash, &sha256);
	//int ret = mbedtls_sha256( ( unsigned char * )raw_data_and_secret.c_str( ), raw_data_and_secret.size( ), hash, 0 );

	auto pass = details::base64_encode( hash, SHA256_DIGEST_LENGTH );

	char out_buf[ 1024 ];
	hffix::message_writer wr{ out_buf, sizeof( out_buf ) };

	wr.push_back_header( "FIX.4.4" );
	wr.push_back_string( fix_spec::MsgType, "A" );
	wr.push_back_int( fix_spec::HeartBtInt, 30 );
	wr.push_back_string( fix_spec::Username, user );
	wr.push_back_string( fix_spec::Password, pass );
	wr.push_back_string( fix_spec::RawData, raw_data );
	wr.push_back_string( fix_spec::ResetSeqNumFlag, "Y");
	wr.push_back_trailer( );

	std::string_view buf{ out_buf, wr.message_size( ) };
	spdlog::info( "buf: {}, len: {}", buf, wr.message_size( ) );

	session->write( buf );
}

int main( ) {
	config::cli_fix_cfg_t cfg{ };

	if ( !config::get_cli_config( "/home/aks/workspace/CPP/fix_parse/bin/fix.json", cfg ) ) {
		spdlog::error( "failed to parse client config file" );
		return 0;
	}

	fix::fix_client cli( "tcp_client", false );
	auto &ctx = cli.ctx( );
	auto &log = cli.log( );

	cli.set_on_connect( con_cb );

	for ( auto &s : cfg.targets ) {
		const auto cur = s.ip.find( ':' );
		if ( cur == std::string::npos ) {
			log->warn( "invalid address specified in config file" );
			continue;
		}
		auto ip = s.ip.substr( 0, cur );
		auto port = s.ip.substr( cur + 1 );

		log->info( "attempting to connect to {}:{}", ip, port );

		int ret = cli.connect( ip, atoi( port.data( ) ) );
		if ( ret != 0 ) {
			log->error( "failed to connect to {}:{}, ret: {}", ip, port, ret );
		}
	};

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 1; ++i )
		std::thread( consumer, std::ref( cli ) ).detach( );

	int ret = cli.run( );

	return ret;
}