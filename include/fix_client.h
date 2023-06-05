#pragma once

#include "include.h"

#include "client.h"

#include "fix_session.h"

#include "fix_parse.h"

namespace fix {
    struct fix_client_context_t {
        tbb::concurrent_unordered_multimap< std::string, fix::fix_session > active_sessions; // target_id -> fix_session
    };

    class fix_client : public net::tcp_client {
      public:
        fix_client( const std::string_view log_name, bool to_file );

        auto &fix_ctx( ) { return m_fix_ctx; }

      private:
        std::shared_ptr< fix_client_context_t > m_fix_ctx;

        tbb::flow::function_node< message::net_msg_t, message::fix_message_t > m_processor;
        tbb::flow::function_node< message::fix_message_t > m_post_processor;
        tbb::flow::sequencer_node< message::fix_message_t > m_sequencer;
    };

    struct post_message_processor_t {
        fix_client &client;
        post_message_processor_t( fix_client &p_client ) : client{ p_client } {}

        void operator( )( message::fix_message_t msg ) {
            auto &ctx = client.ctx( );
            auto &log = client.log( );

            log->info( "got fix msg: fd: {}, fix version: {}.{}, sequence: {}", msg.fd, msg.fix_major, msg.fix_minor,
                       msg.seq );
        }
    };

    struct message_sequencer_t {
        fix_client &client;
        message_sequencer_t( fix_client &p_client ) : client{ p_client } {}

        int operator( )( message::fix_message_t msg ) { return 1; }
    };

    struct message_processor_t {
        fix_client &client;

        message_processor_t( fix_client &p_client ) : client{ p_client } {}

        message::fix_message_t operator( )( message::net_msg_t msg ) {
            auto &ctx = client.ctx( );
            auto &log = client.log( );

            const auto view = msg.cast_to< message::message_buf_t >( );

            // copy, remove later
            std::string buf( view.size( ), '\0' );
            memcpy( buf.data( ), view.data( ), view.size( ) );

            ctx->bufpool.release( view.data( ) );

            auto server_session = ctx->targets[ msg.tag( ) ];
            if ( server_session->state( ) < net::session_state::Idle ) {
                return { };
            }

            fix::fix_message_t rd{ buf.data( ), buf.data( ) + buf.size( ) };

            message::fix_message_t ret{ };
            ret.fd = server_session->fd( );

            auto cur = rd.begin( );
            if ( cur->tag != fix_spec::BeginString ) {
                log->warn( "invalid begin string" );
                return { };
            }

            auto fix_ver = cur->val.as_str( );
            auto delim = fix_ver.find( '.' );
            if ( delim != std::string::npos ) {
                ++delim;
                ret.fix_major = details::atoi( &fix_ver[ delim ], &fix_ver[ delim + 1 ] );
                delim += 2;
                ret.fix_minor = details::atoi( &fix_ver[ delim ], cur->val.end );
            }

            ++cur;
            if ( cur->tag != fix_spec::BodyLength ) {
                log->warn( "invalid body length" );
                return { };
            }

            ret.body_len = cur->val.as_int( );

            ++cur;
            if ( cur->tag != fix_spec::MsgType ) {
                log->warn( "invalid msg type" );
                return { };
            }

            ++cur;

            for ( ; cur < rd.end( ); ++cur ) {
                switch ( cur->tag ) {
                    case fix_spec::MsgSeqNum:
                        ret.seq = cur->val.as_int( );
                        break;
                    default:
                        break;
                }
            }

            return ret;
        }
    };
}; // namespace fix