#pragma once
#include <string>
#include "entity.hpp"

namespace qimq {
	template <class Derived, typename T>
	class storage_t {
	public:
		int add(const int64_t& key, T val) {
			return static_cast<Derived*>(this)->add(key, val);
		}

		T& get(const int64_t& key) {
			return static_cast<Derived*>(this)->get(key);
		}

		bool remove(const int64_t& key) {
			return static_cast<Derived*>(this)->remove(key);
		}

		int64_t size() {
			return static_cast<Derived*>(this)->size();
		}

		bool empty() {
			return static_cast<Derived*>(this)->empty();
		}

		bool has(const int64_t& key) {
			static_cast<Derived*>(this)->has(key);
		}
	};
}