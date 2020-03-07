#include <iostream>
#include <rpc_client.hpp>
#include <chrono>
#include <codec.h>
#include <string>
#include <atomic>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;

struct result {
    int code;
    int64_t msg_id;
    std::string data;
    std::string msg;

    MSGPACK_DEFINE(code, msg_id, data, msg);
};

bool test_pull_ack()
{
    rpc_client client("127.0.0.1", 9000);
    bool r = client.connect();
    REQUIRE(r);

    std::string uuid = "3345-d91c-4b9f-9129-101597cd3dd9";
    bool isRetry = false;

    client.subscribe("pull", uuid, [&](string_view data) {
        isRetry = true;
    });

    // assume pull success
    int msgId = 0;
    auto retVal = client.call<std::string>("pull", msgId, uuid);
    REQUIRE(!retVal.empty());

    try {
        result consume_result{ 0, msgId };
        client.call<void>("pull_ack", msgId, consume_result);
    }
    catch (std::exception & e) {
        std::cout << "some error:" << e.what();
        return false;
    }

    std::cout << "consuming: " << retVal << std::endl;

    std::thread thd([&client]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(25000));
    });

    thd.join();
    return (isRetry == false);
}

TEST_CASE("test pull ack")
{
    CHECK(test_pull_ack() == true);
}