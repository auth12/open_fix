#pragma once

#include "include.h"

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>


namespace message {
    using net_msg_t = tbb::flow::tagged_msg< int, std::span< char > >;

    class fix_rule {};
    class fix_message {
        
    };
};