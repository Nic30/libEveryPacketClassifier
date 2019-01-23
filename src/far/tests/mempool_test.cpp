#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE mempool_test
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <thread>

#include "../mempool.h"

class TestObj final: public ObjectWithStaticMempool<TestObj, 32> {

public:
	size_t i = 0;
	TestObj(size_t i) :
			i(i) {
	}
};

BOOST_AUTO_TEST_SUITE( mempool_testsuite )

BOOST_AUTO_TEST_CASE( simple_new_and_delete ) {

	std::array<TestObj*, 32> items;
	for (int a = 0; a < 2; a++) {
		for (size_t i = 0; i < 32; i++) {
			items[i] = new TestObj(i);
		}
		for (size_t i = 0; i < 32; i++) {
			BOOST_CHECK_EQUAL(items[i]->i, i);
		}
		for (auto i : items) {
			delete i;
		}
	}
}

BOOST_AUTO_TEST_CASE( simple_new_and_delete_8threads ) {
	constexpr size_t THREADS = 8;
	std::array<std::thread*, THREADS> threads;
	std::array<bool, THREADS> error_flags;

	auto test_alloc = [](size_t id, std::array<bool, THREADS> * error_flags) {
		constexpr size_t N = 4;
		std::array<TestObj*, N> items;
		for (int a = 0; a < 500; a++) {
			for (size_t i = 0; i < N; i++) {
				items[i]= new TestObj(i);
			}
			for (size_t i = 0; i < N; i++) {
				(*error_flags)[id] |= items[i]->i != i;
			}
			for (auto i: items) {
				delete i;
			}
		}
	};
	for (size_t i = 0; i < threads.size(); i++) {
		error_flags[i] = false;
		threads[i] = new std::thread(test_alloc, i, &error_flags);
	}

	for (auto t : threads)
		t->join();

	for (size_t i = 0; i < threads.size(); i++) {
		BOOST_CHECK_EQUAL(error_flags[i], false);
	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
