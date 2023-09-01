#pragma once

#include "include.h"

#include <set>

namespace market {
	enum order_side : uint8_t { buy = 0, sell };

	struct entry_t {
		float px;
		float qty;
	};

	static bool operator<( const entry_t &l, const entry_t &r ) { return l.px < r.px; }
	static bool operator>( const entry_t &l, const entry_t &r ) { return l.px > r.px; }

	struct book_t {
		std::set< entry_t, std::less< entry_t > > sells;
        std::set< entry_t, std::greater< entry_t > > buys;


        void register_entry( const entry_t &e, const order_side side ) {
            if( side == buy ) {
                buys.emplace( e );
            } else {
                sells.emplace( e );
            }
        }
	};

	struct order_t {};
}; // namespace market