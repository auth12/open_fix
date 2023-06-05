#include "include.h"

#include "fix_client.h"

fix::fix_client::fix_client( const std::string_view log_name, bool to_file )
    : net::tcp_client{ log_name, to_file }, m_fix_ctx{ std::make_shared< fix_client_context_t >( ) },
      m_sequencer{ graph( ), message_sequencer_t( *this ) },
      m_processor{ graph( ), tbb::flow::unlimited, message_processor_t( *this ) },
      m_post_processor{ graph( ), tbb::flow::serial, post_message_processor_t( *this ) } {
    tbb::flow::make_edge( queue_node( ), m_processor );
    tbb::flow::make_edge( m_processor, m_post_processor );
}