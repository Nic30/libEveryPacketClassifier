#include "threadPool.h"

ThreadPool::ThreadPool(size_t pool_size) :
		work_ios(ios), threads_free(pool_size) {
	for (size_t i = 0; i < pool_size; ++i)
		thread_grp.create_thread(
				boost::bind(&boost::asio::io_service::run, &ios));
}

ThreadPool::~ThreadPool() {
	ios.stop();
	try {
		thread_grp.join_all();
	} catch (const std::exception&) {
	}
}

bool ThreadPool::empty() {
	boost::unique_lock<boost::mutex> lock(mx);
	return threads_free == thread_grp.size();
}

void ThreadPool::wrap_job(boost::function<void()> job) {
	job();
	boost::unique_lock<boost::mutex> lock(mx);
	++threads_free;
}
