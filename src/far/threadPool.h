#pragma once

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <iostream>

class ThreadPool {
private:
	boost::asio::io_service ios;
	boost::asio::io_service::work work_ios;
	boost::thread_group thread_grp;
	unsigned short threads_free;
	boost::mutex mx;
public:

	ThreadPool(size_t pool_size);
	~ThreadPool();

	template<typename Job>
	bool run_job(Job job) {
		boost::unique_lock<boost::mutex> lock(mx);

		if (0 == threads_free)
			return false;
		--threads_free;

		ios.post(
				boost::bind(&ThreadPool::wrap_job, this,
						boost::function<void()>(job)));
		return true;
	}
	bool empty();

private:
	void wrap_job(boost::function<void()> job);
};
