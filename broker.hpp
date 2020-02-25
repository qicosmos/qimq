#pragma once
#include <rpc_server.h>
#include "entity.hpp"
#include "mem_storage.hpp"
using namespace rest_rpc;
using namespace rpc_service;

namespace qimq {
	template<typename T = mem_storage>
	class broker_t {
	public:
		broker_t(short port, size_t size, size_t timeout_seconds = 15, 
			size_t check_seconds = 10): rpc_server_(port, size, timeout_seconds){
			init();
		}

		void run() {
			rpc_server_.run();
		}

		send_result send_msg(rpc_conn conn, std::string key, std::string val) {
			bool r = store_.add(key, val);
			int code = r ? error_code::add_ok : error_code::add_failed;
			send_result result{ code };
			return result;
		}

		std::string pull(rpc_conn conn, std::string key) {
			auto& val = store_.get(key);
			return val;
		}

		void pull_ack(rpc_conn conn, std::string key, consume_result result) {
			if (result.code != 0) {
				std::cout << "pull error\n";
			}
			std::cout << "key: " << key << " has been got by consumer\n";
		}

		void consume_ack(rpc_conn conn, std::string key, consume_result result) {
			if (result.code != 0) {
				std::cout << "consume error\n";
			}

			std::cout << "msg has been consumed\n";

			bool r = store_.remove(key);
			if (r) {
				std::cout << "remove key: "<<key<<" ok\n";
			}
			else {
				std::cout << "remove key: " << key << " failed\n";
			}
		}
	private:
		void init() {
			rpc_server_.register_handler("send_msg", &broker_t::send_msg, this);
			rpc_server_.register_handler("pull", &broker_t::pull, this);
			rpc_server_.register_handler("pull_ack", &broker_t::pull_ack, this);
			rpc_server_.register_handler("consume_ack", &broker_t::consume_ack, this);
		}

		rpc_server rpc_server_;
		T store_;
	};
}