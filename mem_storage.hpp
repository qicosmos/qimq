#pragma once
#include <shared_mutex>
#include <map>
#include "storage.hpp"

namespace qimq {
	static std::string EMPTY_STR = "";
	class mem_storage : public storage_t<mem_storage, std::string> {
	public:

		[[nodiscard]] bool add(const int64_t& key, std::string val) {
			std::unique_lock lock(mtx_);
			map_.emplace(std::move(key), std::move(val));
			return true;
		}

		std::string& get(const int64_t& key) {
			std::unique_lock lock(mtx_);
			auto it = map_.find(key);
			if (it == map_.end())
				return EMPTY_STR;

			return it->second;
		}

		[[nodiscard]] bool remove(const int64_t& key) {
			std::unique_lock lock(mtx_);
			map_.erase(key);
			return true;
		}

		[[nodiscard]] int64_t size() {
			std::unique_lock lock(mtx_);
			return map_.size();
		}

		[[nodiscard]] bool empty() {
			std::unique_lock lock(mtx_);
			return map_.empty();
		}

		[[nodiscard]] bool has(const int64_t& key) {
			std::unique_lock lock(mtx_);
			return map_.find(key) != map_.end();
		}

	private:
		std::shared_mutex mtx_;
		std::map<int64_t, std::string> map_;
	};
}