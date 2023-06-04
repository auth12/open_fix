#pragma once

#include "session.h"

namespace fix {
    class fix_session : public net::tcp_session {
        fix_session( ) = default;
        fix_session( const std::string_view p_sender_id, const std::string_view p_target_id, const int fix_min,
                     const int fix_maj )
            : m_sender_id{ p_sender_id }, m_target_id{ p_target_id }, m_fix_minor{ fix_min }, m_fix_major{ fix_maj },
              m_next_in{ 1 } {}

      private:
        int m_fix_minor, m_fix_major;
        std::atomic< int > m_next_in;

        std::string m_sender_id, m_target_id;
    };
}; // namespace fix
