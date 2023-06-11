#pragma once

#include "include.h"

#include <session.h>

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>

namespace message {
	struct net_msg_t {
		net_msg_t( ) : buf{ nullptr }, len{ 0 }, session{ nullptr } {}

		net_msg_t( const net_msg_t & ) = delete;
        net_msg_t &operator=( const net_msg_t & ) = delete;

		net_msg_t( net_msg_t && ) = default;
		net_msg_t &operator=( net_msg_t && ) = default;

		char *buf;
		net::tcp_session *session;
		size_t len;
	};

	class fix_rule {};
	struct fix_message_t {
		int fd = 0;

		int seq = 0;
		int body_len = 0;

		int fix_major, fix_minor = 0;
	};
}; // namespace message