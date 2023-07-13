#include "include.h"

#include <tcp_server.h>

#include <fix_parse.h>

#define HOST "127.0.0.1"
#define PORT "1515"

void consumer( net::tcp_server &srv ) {
	auto ctx = srv.ctx( );
	auto log = srv.log( );

	while ( 1 ) {
		auto msg = ctx->msg_queue.pop( );

		auto session = ctx->sessions[ msg.fd ];

		std::string buf{ msg.buf, msg.len };
		ctx->bufpool.release( msg.buf );

		fix::fix_message_t rd{ buf };
		for ( auto &f : rd ) {
			log->info( "{}->{}", f.tag, f.val.as_string( ) );
		}
	}
}

constexpr char fix_buf[] = "8=FIX.4.4\0019=178\00135=W\00149=SENDER\00156=RECEIVER\00134=123\00152=20230517-09:30:00."
						   "000\00155=EUR/"
						   "USD\001262=1\001268=2\001269=0\0016=1.2345\001271=100000\00110=080\001";

void on_timer_cb( uv_timer_t *timer ) {
	auto srv = ( net::tcp_server * )timer->loop->data;

	auto ctx = srv->ctx( );
	auto log = srv->log( );
	for ( auto &[ fd, session ] : ctx->sessions ) {
		session->write( fix_buf );
	}
}

int main( ) {
	net::tcp_server srv( "tcp_server", false );
	auto ctx = srv.ctx( );
	auto log = srv.log( );

	if ( srv.bind( HOST, PORT ) != 0 ) {
		log->error( "failed to bind" );
		return 0;
	}

	log->info( "listening on {}:{}", HOST, PORT );

	for ( int i = 0; i < std::thread::hardware_concurrency( ) - 1; ++i )
		std::thread( consumer, std::ref( srv ) ).detach( );

	uv_timer_t timer;
	uv_timer_init( srv.loop( ), &timer );
	uv_timer_start( &timer, on_timer_cb, 0, 100 );

	return srv.run( );
};