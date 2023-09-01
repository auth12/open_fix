#pragma once

#include "include.h"

#include "session.h"

namespace net {
	struct tcp_msg_t {
		tcp_msg_t( ) {}
		tcp_msg_t( char *ptr, const size_t plen ) : buf{ ptr }, len{ plen } {}

		char *buf;
		size_t len;
	};

	struct tagged_tcp_msg_t : tcp_msg_t {
		int fd = 0;
	};
}; // namespace net