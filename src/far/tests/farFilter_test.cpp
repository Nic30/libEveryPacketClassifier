#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE farFilter_test
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../farFilter.h"
#include "IO/InputReader.h"

using Filter = FaRFilter<uint32_t, unsigned, 5>;
using Filter8c = FaRFilter<uint32_t, unsigned, 5, 2>;
const int N = 2;

BOOST_AUTO_TEST_SUITE( farFilter_testsuite )

BOOST_AUTO_TEST_CASE( add_100_rules ) {
	Filter t;
	//auto fn = "tests/rulesets/acl1_100";
	//auto fn =  "../classbench-ng/generated/fw1_2000";
	auto fn =  "../classbench-ng/generated/acl1_10000";

	auto rules = InputReader::ReadFilterFile(fn);

	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; i++) {

		for (auto & r : rules) {
			t.insert(r);
		}
		for (auto & r : rules) {
			t.remove(r);
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	std::cout << "sequential " << time / std::chrono::milliseconds(1) << "ms"
			<< std::endl;
	//for (auto & r: t.get_memory_report()) {
	//	std::cout << r << std::endl;
	//}
	//std::ofstream myfile("far_100.dot");
	//myfile << t << std::endl;
	//myfile.close();
}

BOOST_AUTO_TEST_CASE( add_100_rules_8c ) {
	Filter8c t;
	//auto fn = "tests/rulesets/acl1_100";
	//auto fn =  "../classbench-ng/generated/fw1_2000";
	auto fn =  "../classbench-ng/generated/acl1_10000";

	auto rules = InputReader::ReadFilterFile(fn);

	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; i++) {
		for (auto & r : rules) {
			t.insert(r);
		}
		for (auto & r : rules) {
			t.remove(r);
		}
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	std::cout << "parallel   " << time / std::chrono::milliseconds(1) << "ms"
			<< std::endl;
	//std::ofstream myfile("far_100.dot");
	//myfile << t << std::endl;
	//myfile.close();
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
