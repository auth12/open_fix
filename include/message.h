#pragma once

#include "include.h"

#include <session.h>

namespace net {
	struct tcp_msg_t {
		tcp_msg_t( const tcp_msg_t & ) = delete;
		tcp_msg_t &operator=( const tcp_msg_t & ) = delete;

		tcp_msg_t( tcp_msg_t && ) = default;
		tcp_msg_t &operator=( tcp_msg_t && ) = default;

		tcp_msg_t( ) {}
		tcp_msg_t( char *ptr, const size_t len ) : buf{ ptr, len } {}

		std::span< char > buf;
	};

	struct tagged_tcp_msg_t : tcp_msg_t {
		int fd = -1;
	};
}; // namespace net