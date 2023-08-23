#pragma once

#include "include.h"

#include "fix_client.h"

namespace fix {
	struct lmax_t {
		std::string fix_ver, target_id, sender_id;
		std::atomic_int seq, state;

		lmax_t( ) : seq{ 1 } {}

		int next_seq( ) { return seq.fetch_add( 1, std::memory_order_release ); }

		size_t create_logon( char *buf, const size_t buf_size ) {
			fix::fix_writer wr{ buf, buf_size };

			wr.push_header( fix_ver );
			wr.push_field( fix_spec::MsgType, 'A' );

			wr.push_field( fix_spec::TargetCompID, target_id );
			wr.push_field( fix_spec::SenderCompID, sender_id );
			wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
			wr.push_field( fix_spec::MsgSeqNum, next_seq( ) );
			wr.push_field( fix_spec::ResetSeqNumFlag, 'Y' );
			wr.push_field( fix_spec::EncryptMethod, 0 );
			wr.push_field( fix_spec::HeartBtInt, 30 );

			wr.push_field( fix_spec::Username, sender_id );
			wr.push_field( fix_spec::Password, sender_id );
			wr.push_trailer( );

			return wr.size( );
		}

		size_t create_heartbeat( char *buf, const size_t buf_size ) {
			fix::fix_writer wr{ buf, buf_size };
			wr.push_header( fix_ver );
			wr.push_field( fix_spec::MsgType, '0' );

			wr.push_field( fix_spec::TargetCompID, target_id );
			wr.push_field( fix_spec::SenderCompID, sender_id );

			wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
			wr.push_field( fix_spec::MsgSeqNum, next_seq( ) );
			wr.push_trailer( );

			return wr.size( );
		}

		size_t create_market_request( char *buf, const size_t buf_size ) {
			fix::fix_writer wr{ buf, buf_size };

			wr.push_header( fix_ver );
			wr.push_field( fix_spec::MsgType, 'V' );

			wr.push_field( fix_spec::TargetCompID, target_id );
			wr.push_field( fix_spec::SenderCompID, sender_id );

			wr.push_timestamp( fix_spec::SendingTime, std::chrono::system_clock::now( ) );
			wr.push_field( fix_spec::MsgSeqNum, next_seq( ) );

			wr.push_field( fix_spec::MDReqID, "TEST" );
			wr.push_field( fix_spec::SubscriptionRequestType, 1 );
			wr.push_field( fix_spec::MarketDepth, 0 );

			wr.push_field( fix_spec::NoMDEntryTypes, 2 );
			wr.push_field( fix_spec::MDEntryType, 0 );
			wr.push_field( fix_spec::MDEntryType, 1 );

			wr.push_field( fix_spec::NoRelatedSym, 1 );

			wr.push_field( fix_spec::SecurityID, 5002 );
			wr.push_field( fix_spec::SecurityIDSource, 8 );

			wr.push_trailer( );

			return wr.size( );
		}
	};

	using lmax_fix_client = fix::fix_client< lmax_t >;
}; // namespace fix