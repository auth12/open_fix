#pragma once

#include "include.h"

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>

namespace market {
	struct order_t {
		double price, volume;

		bool operator<( const order_t &o ) const { return price < o.price; }
        bool operator>( const order_t &o ) const { return price > o.price; }
		bool operator==( const order_t &o ) const { return price == o.price; }
	};

	struct orderbook_t {
		absl::btree_set< order_t, std::greater< order_t > > buy;
        absl::btree_set< order_t, std::less< order_t > > sell;


	};
}; // namespace market