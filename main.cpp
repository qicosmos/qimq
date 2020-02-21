#include <iostream>
#include "broker.hpp"
using namespace qimq;

int main() {
	broker_t broker(9000, std::thread::hardware_concurrency());
	auto& rpc_server = broker.get_rpc_server();

	//rpc_server.register_handler
	rpc_server.register_handler("hello", [](rpc_conn conn, std::string str) {

	});

	broker.run();
	return 0;
}