#pragma once

#include "include.h"

#include <session.h>

namespace net {
	struct tcp_msg_t {
		tcp_msg_t( ) {}
		tcp_msg_t( char *ptr, const size_t plen ) : buf{ ptr }, len{ plen }, timestamp{ 0 } {}
		tcp_msg_t( char *ptr, const size_t plen, const int64_t ts ) : buf{ ptr }, len{ plen }, timestamp{ ts } {}

		char *buf;
		size_t len;
		int64_t timestamp;
	};

	struct tagged_tcp_msg_t : tcp_msg_t {
		int fd = -1;
	};

	struct fix_message_handler_t {
		void create_logon_msg(  ) {

		}
	};
}; // namespace net