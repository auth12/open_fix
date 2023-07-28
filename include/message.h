#pragma once

#include "include.h"

#include <session.h>

namespace net {
	struct tcp_msg_t {
		tcp_msg_t( ) {}
		tcp_msg_t( char *ptr, const size_t len ) : buf{ ptr, len } {}
		tcp_msg_t( char *ptr, const size_t len, const int64_t ts ) : buf{ ptr, len }, timestamp{ ts } {}

		std::span< char > buf;
		int64_t timestamp;
	};

	struct tagged_tcp_msg_t : tcp_msg_t {
		int fd = -1;
	};
}; // namespace net