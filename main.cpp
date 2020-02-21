#include <iostream>
#include "broker.hpp"
using namespace qimq;

int main() {
	broker_t broker(9000, std::thread::hardware_concurrency());
	broker.run();
	return 0;
}