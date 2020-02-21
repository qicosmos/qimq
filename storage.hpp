#pragma once
#include <string>

namespace qimq {
	template <class Derived, typename T>
	class storage_t {
	public:
		bool add(std::string key, T val) {
			return static_cast<Derived*>(this)->add(key, val);
		}

		T& get(const std::string& key) {
			return static_cast<Derived*>(this)->get(key);
		}

		bool remove(std::string key) {
			return static_cast<Derived*>(this)->remove(key);
		}
	};
}