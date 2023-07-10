#pragma once


#include "include.h"

#include "json.h"

namespace config {
    struct target_t {
        std::string target_id, sender_id, ip, fix_ver;
        std::vector< std::string > market_subscriptions;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( target_t, target_id, sender_id, ip, fix_ver, market_subscriptions )

    struct cli_fix_cfg_t {
        std::string log_name;
        bool to_file;
        std::vector< target_t > targets;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( cli_fix_cfg_t, targets, log_name, to_file )

    static bool get_cli_config( const std::string_view cfg_name, cli_fix_cfg_t &out ) {
        std::ifstream f( cfg_name.data( ) );
        if( !f.good( ) ) {
            return false;
        }

        auto j = nlohmann::json::parse( f );
        if( j.empty( ) ) {
            return false;
        }

        nlohmann::from_json( j[ "client" ], out );


        return true;
    };
};