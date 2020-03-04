#pragma once
#include "storage.hpp"
#include "consts.h"
#ifdef USE_ROCKSDB
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace rocksdb;
#endif // USE_ROCKSDB

#include "itoa_jeaiii.hpp"

namespace qimq {
#ifdef USE_ROCKSDB
	class file_storage : public storage_t<file_storage, std::string> {
	public:
		~file_storage() {
			delete db_;
		}

		file_storage() {
			Options options;
			// Optimize RocksDB. This is the easiest way to get RocksDB to perform well
			options.IncreaseParallelism();
			//options.OptimizeLevelStyleCompaction();
			// create the DB if it's not already present
			options.create_if_missing = true;

			// open DB
			Status s = DB::Open(options, "qimq_store", &db_);
			if (!s.ok()) {
				std::cout << s.ToString() << "\n";
			}
		}

		[[nodiscard]] error_code add(const int64_t& key, std::string val) {
			char temp[20];
			i64toa_jeaiii(key, temp);

			std::string value;
			auto s1 = db_->Get(ReadOptions(), temp, &value);
			if (!s1.IsNotFound()) {
				return error_code::has_exist;
			}

			auto s = db_->Put(w_options_, temp, std::move(val));
			return s.ok() ? error_code::ok : error_code::add_failed;
		}

		std::string& get(const int64_t& key) {
			char temp[20];
			i64toa_jeaiii(key, temp);

			std::string value;
			auto s1 = db_->Get(ReadOptions(), temp, &value);
			if (s1.IsNotFound()) {
				return EMPTY_STR;
			}

			return value;
		}

		[[nodiscard]] bool remove(const int64_t& key) {
			char temp[20];
			i64toa_jeaiii(key, temp);

			auto s = db_->Delete(w_options_, temp);
			return s.ok();
		}

		[[nodiscard]] int64_t size() {
			int64_t size = 0;
			rocksdb::Iterator* it = db_->NewIterator(r_options_);
			for (it->SeekToFirst(); it->Valid(); it->Next()) {
				size++;
				//cout << it->key().ToString() << ": " << it->value().ToString() << endl;
			}

			delete it;
			return it->status().ok() ? size : -1;
		}

		[[nodiscard]] bool empty() {
			rocksdb::Iterator* it = db_->NewIterator(r_options_);
			it->SeekToFirst();
			bool is_empty = !(it->Valid());
			delete it;
			return is_empty;
		}

		[[nodiscard]] bool has(const int64_t& key) {
			char temp[20];
			i64toa_jeaiii(key, temp);

			std::string value;
			auto s = db_->Get(ReadOptions(), temp, &value);
			return !s.IsNotFound();
		}

	private:
		//std::shared_mutex mtx_;
		DB* db_;
		const WriteOptions w_options_{};
		const rocksdb::ReadOptions r_options_{};
	};
#endif // USE_ROCKSDB
}