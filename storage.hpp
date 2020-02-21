#pragma once
#include <string>

namespace qimq {
	template <class Derived, typename T>
	class storage_t {
	public:

		bool add(std::string key, T val) {
			static_cast<Derived*>(this)->add(key, val);
			return true;
		}


		T get(std::string key) {
			static_cast<Derived*>(this)->get(key);
			return {};
		}

		bool remove(std::string key) {
			static_cast<Derived*>(this)->remove(key);
			return true;
		}
	};
}