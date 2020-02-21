#pragma once
#include <mutex>
#include <map>
#include "storage.hpp"

namespace qimq {
	class mem_storage : public storage_t<mem_storage, std::string> {
	public:

		bool add(std::string key, std::string val) {
			std::unique_lock lock(mtx_);
			map_.emplace(key, val);
			return true;
		}

		std::string get(std::string key) {
			std::unique_lock lock(mtx_);
			auto it = map_.find(key);
			if (it == map_.end())
				return "";

			return it->second;
		}

		bool remove(std::string key) {
			std::unique_lock lock(mtx_);
			map_.erase(key);
			return true;
		}

	private:
		std::mutex mtx_;
		std::map<std::string, std::string> map_;
	};
}