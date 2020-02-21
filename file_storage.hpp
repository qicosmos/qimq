#pragma once
#include "storage.hpp"

namespace qimq {
	class file_storage : public storage_t<file_storage, std::string> {
	public:

		bool add(std::string key, std::string val) {
			return true;
		}

		std::string get(std::string key) {
			return {};
		}

		bool remove(std::string key) {
			return true;
		}

	private:
	};
}