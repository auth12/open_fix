#pragma once


namespace net {
    enum tcp_error: int {
        ok = 0,
        getaddrinfo_err = -1,
        connect_err = -2,
        sock_err = -3
    };
};