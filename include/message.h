#pragma once

#include "include.h"

#include <session.h>

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>

namespace message {
	struct net_msg_t {
		net_msg_t( ) : buf{ nullptr }, len{ 0 }, fd{ -1 } {}
		net_msg_t( const int p_fd, char *p_buf, const size_t p_len ) : buf{ p_buf }, len{ p_len }, fd{ p_fd } {}

		net_msg_t( const net_msg_t & ) = delete;
		net_msg_t &operator=( const net_msg_t & ) = delete;

		net_msg_t( net_msg_t && ) = default;
		net_msg_t &operator=( net_msg_t && ) = default;

		int fd;
		char *buf;
		size_t len;
	};

	inline static std::unique_ptr< net_msg_t > make_msg( const int fd, char *buf, const size_t len ) {
		return std::make_unique< net_msg_t >( fd, buf, len );
	}

	class fix_rule {};
	struct fix_message_t {
		int fd = 0;

		int seq = 0;
		int body_len = 0;

		int fix_major, fix_minor = 0;
	};
}; // namespace message