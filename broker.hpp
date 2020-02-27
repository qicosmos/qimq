#pragma once
#include <rpc_server.h>
#include <shared_mutex>
#include "entity.hpp"
#include "mem_storage.hpp"
using namespace rest_rpc;
using namespace rpc_service;

namespace qimq {
	template<typename T = mem_storage>
	class broker_t {
	public:
		broker_t(short port, size_t size, size_t timeout_seconds = 15, 
			size_t check_seconds = 10): rpc_server_(port, size, timeout_seconds), worker_(ios_){
			init();
		}

		~broker_t() {
			ios_.stop();
			wait_thd_.join();
		}

		void run() {
			rpc_server_.run();
		}

		send_result send_msg(rpc_conn conn, int64_t msg_id, std::string val) {
			auto code = store_.add(msg_id_, val);
			send_result result{ code, msg_id_++ };
			return result;
		}

		std::string pull(rpc_conn conn, int64_t msg_id, std::string client_id) {
			auto& val = store_.get(msg_id);

			start_wait_ack(conn, msg_id, std::move(client_id), val);

			return val;
		}

		void start_wait_ack(rpc_conn conn, int64_t msg_id, std::string client_id, const std::string& val) {
			std::shared_ptr<wait_t> wait = std::make_shared<wait_t>(ios_, 5000);
			std::weak_ptr<wait_t> wp(wait);
			{
				std::unique_lock lock(wait_mtx_);
				wait_ack_map_.emplace(msg_id, wait);
			}

			wait->set_callback([this, conn, wp, msg_id, &val, client_id = std::move(client_id)](int try_times) {
				if (try_times <= 5) {
					auto call = wp.lock();
					if (call) {
						std::cout << "time out, try times: " << try_times << " " << call.use_count() << "\n";
						auto c = conn.lock();
						if (c) {
							//retry to push message to the client
							rpc_server_.publish_by_token("pull", std::move(client_id), send_result{ok, msg_id, val});
							call->start_timer();
						}
					}
				}
				else {
					std::cout << "retry 5 times failed\n";
				}
			});
			wait->start_timer();
		}

		void pull_ack(rpc_conn conn, int64_t msg_id, consume_result result) {
			decltype(wait_ack_map_.begin()) it;

			{
				std::unique_lock lock(wait_mtx_);
				it = wait_ack_map_.find(msg_id);
				if (it == wait_ack_map_.end()) {
					//the msg_id is not exist
					return;
				}
			}

			auto wait = it->second.lock();
			wait->cancel();

			{
				std::unique_lock lock(wait_mtx_);
				wait_ack_map_.erase(it);
			}			

			if (result.code != 0) {
				std::cout << "pull error\n";
			}
			std::cout << "msg_id: " << msg_id << " has been got by consumer\n";
		}

		void consume_ack(rpc_conn conn, int64_t msg_id, consume_result result) {
			if (result.code != 0) {
				std::cout << "consume error\n";
			}

			std::cout << "msg has been consumed\n";

			bool r = store_.remove(msg_id);
			if (r) {
				std::cout << "remove msg_id: "<< msg_id <<" ok\n";
			}
			else {
				std::cout << "remove msg_id: " << msg_id << " failed\n";
				std::cout << "remove msg_id: " << msg_id << " failed\n";
			}
		}
	private:
		void init() {
			rpc_server_.register_handler("send_msg", &broker_t::send_msg, this);
			rpc_server_.register_handler("pull", &broker_t::pull, this);
			rpc_server_.register_handler("pull_ack", &broker_t::pull_ack, this);
			rpc_server_.register_handler("consume_ack", &broker_t::consume_ack, this);

			wait_thd_ = std::thread([this] {
				ios_.run();

				std::cout << "quit\n";
			});
		}

		class wait_t : asio::noncopyable, public std::enable_shared_from_this<wait_t> {
		public:
			wait_t(asio::io_service& ios, size_t timeout) : timer_(ios),
				timeout_(timeout) {
			}
			~wait_t() {
				std::cout << "destruct\n";
			}
			void start_timer() {
				if (timeout_ == 0) {
					return;
				}

				timer_.expires_from_now(std::chrono::milliseconds(timeout_));
				auto self = this->shared_from_this();
				timer_.async_wait([this, self](boost::system::error_code ec) {
					if (ec) {
						callback(false);
						return;
					}

					has_timeout_ = true;
					callback(true);
				});
			}

			void set_callback(std::function<void(int)> callback) {
				cb_ = std::move(callback);
			}

			bool has_timeout() const {
				return has_timeout_;
			}

			void cancel() {
				if (timeout_ == 0) {
					return;
				}

				boost::system::error_code ec;
				timer_.cancel(ec);
			}

		private:
			void callback(bool has_timeout) {
				if (!has_timeout)
					return;

				try_times_++;

				if (cb_) {
					cb_(try_times_);
				}				
			}

			boost::asio::steady_timer timer_;
			std::function<void(int)> cb_;
			size_t timeout_;
			bool has_timeout_ = false;
			int try_times_ = 0;
		};

		rpc_server rpc_server_;
		T store_;
		std::atomic<std::int64_t> msg_id_ = store_.empty() ? 0 : store_.size();
		boost::asio::io_service ios_;
		boost::asio::io_service::work worker_;
		std::thread wait_thd_;
		std::unordered_map<std::uint64_t, std::weak_ptr<wait_t>> wait_ack_map_;
		std::shared_mutex wait_mtx_;
	};
}