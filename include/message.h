#pragma once

#include "include.h"

#include <session.h>

namespace message {
	struct net_msg_t {
		net_msg_t( ) : buf{ nullptr }, len{ 0 }, fd{ -1 } {}
		net_msg_t( const int &p_fd, char *p_buf, const size_t &p_len ) : buf{ p_buf }, len{ p_len }, fd{ p_fd } {}

		net_msg_t( const net_msg_t & ) = delete;
		net_msg_t &operator=( const net_msg_t & ) = delete;

		net_msg_t( net_msg_t && ) = default;
		net_msg_t &operator=( net_msg_t && ) = default;

		int fd;
		char *buf;
		size_t len;
	};

	
}; // namespace message