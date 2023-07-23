#pragma once

#include "include.h"

#include "json.h"

namespace config {
	struct client_fix_cfg_t {
		bool to_file;
		int port;
		std::string log_name, target_id, sender_id, host, fix_ver;
		std::vector< std::string > market_subscriptions;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( client_fix_cfg_t, target_id, sender_id, host, port, fix_ver,
										market_subscriptions, log_name, to_file )

	static bool get_cli_config( const std::string_view cfg_name, client_fix_cfg_t &out ) {
		std::ifstream f( cfg_name.data( ) );
		if ( !f.good( ) ) {
			return false;
		}

		auto j = nlohmann::json::parse( f );
		if ( j.empty( ) ) {
			return false;
		}
		try {
			nlohmann::from_json( j[ "client" ], out );
		} catch ( std::exception &e ) {
			return false;
		}

		return true;
	};
}; // namespace config