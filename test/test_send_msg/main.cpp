#include <iostream>
#include <rpc_client.hpp>
#include <chrono>
#include <fstream>
#include <codec.h>
#include "../../broker.hpp"

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;

void test_send_msg(int64_t msgid) {
    try {
        rpc_client client("127.0.0.1", 9000);
        bool r = client.connect();
        if (!r) {
            std::cout << "connect timeout" << std::endl;
            return;
        }

        auto result = client.call<qimq::send_result>("send_msg", msgid, "hello");
        std::cout << "msg_id: " << result.msg_id << std::endl;
    }
    catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
}

int main() {
    std::cout << "send msg test start!" << std::endl;
    test_send_msg(0);
    return 0;
}