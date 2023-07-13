#include "include.h"

#include "fix_client.h"

int fix::fix_client::connect( ) {
	for ( auto &t : m_cfg.targets ) {
		const auto cur = t.ip.find( ':' );
		if ( cur == std::string::npos ) {
			m_fix_ctx->log->warn( "invalid address specified in config file" );
			continue;
		}

		auto host = t.ip.substr( 0, cur );
		auto port = t.ip.substr( cur + 1 );

		m_fix_ctx->log->info( "Attempting to connect to {}:{}", host, port );

		int ret = net::tcp_client::connect( host, port );
		if ( ret < net::tcp_error::ok ) {
			m_fix_ctx->log->error( "Failed to connect to {}:{}", host, port );
			continue;
		}

		m_fix_ctx->log->info( "Connected to {}:{}", host, port );

		auto session = std::make_shared< fix_session >( ret, t.target_id, t.sender_id, t.fix_ver );

		m_fix_ctx->active_sessions[ ret ].swap( session );
	}

	return m_fix_ctx->active_sessions.size( );
}