#pragma once
#include <queue>
#include <boost/thread.hpp>

template<typename T>
class concurrent_queue {
private:
	std::queue<T> queue;
	mutable boost::mutex mutex;
	boost::condition_variable condition;
public:
	concurrent_queue(size_t prealoc_size) {
	}
	bool push(T const& data) {
		boost::mutex::scoped_lock lock(mutex);
		queue.push(data);
		lock.unlock();
		condition.notify_one();
		return true;
	}

	bool empty() const {
		boost::mutex::scoped_lock lock(mutex);
		return queue.empty();
	}

	bool pop(T& popped_value) {
		boost::mutex::scoped_lock lock(mutex);
		if (queue.empty()) {
			return false;
		}

		popped_value = queue.front();
		queue.pop();
		return true;
	}

	void pop_blocking(T& popped_value) {
		boost::mutex::scoped_lock lock(mutex);
		while (queue.empty()) {
			condition.wait(lock);
		}

		popped_value = queue.front();
		queue.pop();
	}

};
