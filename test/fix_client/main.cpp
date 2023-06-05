#include "include.h"

#include <fix_client.h>

#include <config.h>

int main( ) {
    config::cli_fix_cfg_t cfg{ };

    if ( !config::get_cli_config( "fix.json", cfg ) ) {
        spdlog::error( "failed to parse client config file" );
        return 0;
    }

    fix::fix_client cli( "tcp_client", false );
    auto &ctx = cli.ctx( );
    auto &log = cli.log( );

    for ( auto &s : cfg.targets ) {
        const auto cur = s.ip.find( ':' );
        if ( cur == std::string::npos ) {
            log->warn( "invalid address specified in config file" );
            continue;
        }
        auto ip = s.ip.substr( 0, cur );
        auto port = s.ip.substr( cur + 1 );

        log->info( "attempting to connect to {}:{}", ip, port );

        int ret = cli.connect( ip, port );
        if ( ret != 0 ) {
            log->error( "failed to connect to {}:{}.", ip, port );
            continue;
        }

        log->info( "connected." );
    };

    if ( ctx->targets.empty( ) ) {
        return 0;
    }

    return cli.run( );
}