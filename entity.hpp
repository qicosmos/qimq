#pragma once
#include <msgpack.hpp>

namespace qimq {
	struct send_result {
		int code;
		int64_t msg_id;
		std::string data;
		std::string msg;

		MSGPACK_DEFINE(code, msg_id, data, msg);
	};

	//struct message_t {
	//	int64_t msg_id;
	//	std::string msg;
	//	MSGPACK_DEFINE(msg_id, msg);
	//};

	using pull_result = send_result;

	using consume_result = send_result;

	enum error_code {
		ok = 0,
		has_exist,
		add_failed,
		at_capacity,

	};
}