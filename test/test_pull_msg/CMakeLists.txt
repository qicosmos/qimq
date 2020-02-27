cmake_minimum_required(VERSION 3.0)
project(test_pull_msg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++17")

include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}
        "../../rest_rpc/include"
        "../../rest_rpc/third/msgpack/include"
        )

add_executable(test_pull_msg main.cpp)