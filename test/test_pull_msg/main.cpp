#include <iostream>
#include <rpc_client.hpp>
#include <chrono>
#include <fstream>
#include <codec.h>
#include <string>

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;

void test_pull_msg(int64_t msgid) {
    try {
        rpc_client client("127.0.0.1", 9000);
        bool r = client.connect();
        if (!r) {
            std::cout << "connect timeout" << std::endl;
            return;
        }

        std::string uuid = "1746a8d4-d91c-4b9f-9129-101597cd3dd9";
        auto result = client.call<std::string>("pull", msgid ,uuid );
        std::cout << result << std::endl;
    }
    catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
}

int main() {
    std::cout << "pull msg test start!" << std::endl;
    test_pull_msg(0);
    return 0;
}