#pragma once


#include "include.h"

#include "json.h"

namespace config {
    struct cli_fix_cfg_t {
        std::string ip, port, target_id, sender_id;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( cli_fix_cfg_t, ip, port, target_id, sender_id )

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