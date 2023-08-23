#include "include.h"

#include "lmax.h"

#include <config.h>

#include <sched.h>
#include <pthread.h>

std::unique_ptr< fix::lmax_fix_client > g_cli;

std::once_flag g_once;

void consume_in( ) {
	auto lmax = g_cli->broker( );

	while ( g_cli->fd( ) > 0 ) {
		fix::fix_msg_t msg{ };
		if ( !g_cli->consume_fix_msg( msg ) ) {
			continue;
		}

		std::call_once( g_once, [ & ] {
			if ( lmax->state == fix::LoggedIn ) {
				g_cli->post_market_request( );
			}
		} );

		g_cli->release_buf( msg.buf );
	}
}

int main( ) {
	config::client_fix_cfg_t cfg{ };

	spdlog::set_level( spdlog::level::debug );

	if ( !config::get_cli_config( "fix.json", cfg ) ) {
		spdlog::error( "Failed to parse client config file" );
		return 0;
	}

	g_cli = std::make_unique< fix::lmax_fix_client >( );
	auto broker = g_cli->broker( );
	broker->fix_ver = cfg.fix_ver;
	broker->target_id = cfg.target_id;
	broker->sender_id = cfg.sender_id;

	const auto host = cfg.host, port = fmt::to_string( cfg.port );

	if ( g_cli->connect( host, port ) != 0 ) {
		SPDLOG_ERROR( "Failed to connect to {}:{}", host, port );

		return 0;
	}

	g_cli->post_logon( );

	std::thread( consume_in ).detach( );

	g_cli->poll( );

	return 0;
}