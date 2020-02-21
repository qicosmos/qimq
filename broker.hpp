#pragma once
#include <rpc_server.h>
#include "entity.hpp"
#include "mem_storage.hpp"
using namespace rest_rpc;
using namespace rpc_service;

namespace qimq {
	class broker_t {
	public:
		broker_t(short port, size_t size, size_t timeout_seconds = 15, 
			size_t check_seconds = 10): rpc_server_(port, size, timeout_seconds){
			init();
		}

		rpc_server& get_rpc_server() {
			return rpc_server_;
		}

		void run() {
			rpc_server_.run();
		}

		publish_result pulish(rpc_conn conn, std::string key, std::string val) {
			//先落地再应答
			store_.add(key, val);
			publish_result result{ 200 };
			return result;
		}
	private:
		void init() {
			rpc_server_.register_handler("publish", &broker_t::pulish, this);
		}

		rpc_server rpc_server_;
		mem_storage store_;
	};
}