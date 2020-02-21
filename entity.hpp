#pragma once
#include <msgpack.hpp>

namespace qimq {
	struct publish_result {
		int code;
		std::string data;
		std::string msg;

		MSGPACK_DEFINE(code, data, msg);
	};
}