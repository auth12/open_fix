#pragma once

#include "include.h"

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>


namespace message {
    using message_buf_t = std::span< char >;
    using net_msg_t = tbb::flow::tagged_msg< int, message_buf_t >;

    class fix_rule {};
    struct fix_message_t {
        int fd = 0;

        int seq = 0;
        int body_len = 0;
        
        int fix_major, fix_minor = 0;
    };
};