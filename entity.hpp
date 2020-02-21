#pragma once
#include <msgpack.hpp>

namespace qimq {
	struct send_result {
		int code;
		std::string data;
		std::string msg;

		MSGPACK_DEFINE(code, data, msg);
	};

	using pull_result = send_result;

	using consume_result = send_result;
}