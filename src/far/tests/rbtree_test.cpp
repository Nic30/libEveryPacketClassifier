#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rbtree_test
#include <boost/test/unit_test.hpp>
#include <functional>
#include "RBTree.h"
#include "fragmented_value.h"
#include <iostream>
#include <fstream>
#include <stdint.h>

using Tree_t = LLRBTree<unsigned>;
using RuleTree_t = LLRBTree<FragmentedValue<MaskedValue<unsigned>, 3>>;
BOOST_AUTO_TEST_SUITE( rbtree_testsuite )

BOOST_AUTO_TEST_CASE( simple_add_and_discard ) {
	Tree_t t;
	unsigned N = 8;

	for (int r = 0; r < 1; r++) {
		N += r * N;
		// add some items
		for (unsigned i = 0; i < N; i++) {
			t.insert(i);
			BOOST_CHECK(t.size() == i + 1);
		}

		//std::cout << "removing 2" << std::endl;
		//std::ofstream myfile("simple_add_and_discard.dot");
		//myfile  << t << std::endl;
		//myfile.close();

		// remove all items
		for (unsigned i = 0; i < N; i++) {
			BOOST_CHECK(t.size() == N - i);
			t.remove(i);
			BOOST_CHECK(t.size() == N - i - 1);
		}
	}

}

BOOST_AUTO_TEST_CASE( add_and_discard_rev_order ) {
	Tree_t t;
	unsigned N = 10;
	// add some items
	for (unsigned i = 10; i < 10 + N; i++) {
		t.insert(i);
		BOOST_CHECK(t.size() == i - 10 + 1);
	}

	//std::ofstream myfile("add_and_discard_rev_order.dot");
	//myfile  << t << std::endl;
	//myfile.close();
	// remove all items in reversed order
	for (unsigned i = 0; i < N; i++) {
		BOOST_CHECK(t.size() == 10 - i);
		unsigned k = 10 + N - i - 1;
		t.remove(k);
	}
}

BOOST_AUTO_TEST_CASE( add_and_discard_fragmentedValue ) {
	RuleTree_t t;
	using K = RuleTree_t::Key_t;

	unsigned N = 8;
	std::vector<K> rules0;
	for (unsigned i = 0; i < N; i++) {
		rules0.push_back(K( { i & 1, (i & 2) >> 1, (i & 4) >> 2 }));
	}

	// add some items
	for (const auto & k : rules0) {
		t.insert(k);
	}

	// remove all items in reversed order
	for (const auto & k : rules0) {
		t.remove(k);
		//std::ofstream myfile("add_and_discard_fragmentedValue.dot");
		//myfile << t << std::endl;
		//myfile.close();
	}
}
//BOOST_AUTO_TEST_CASE( add_duplicit ) {
//	Tree_t t;
//	// add some items
//	for (unsigned i = 0; i < 10; i++) {
//		auto add = t.add(i);
//		BOOST_CHECK(t.size() == i + 1);
//		BOOST_CHECK(add);
//	}
//	for (unsigned i = 0; i < 10; i++) {
//		auto add = t.add(i);
//		BOOST_CHECK(t.size() == 10);
//		BOOST_CHECK(!add);
//	}
//
//	// discard items which are not present in tree
//	for (unsigned i = 0; i < 10; i++) {
//		bool was_discarded = t.discard(i);
//		BOOST_CHECK(was_discarded);
//		BOOST_CHECK(t.size() == 10 - i - 1);
//	}
//}
//
//BOOST_AUTO_TEST_CASE( merge ) {
//	using Vec_t = std::vector<unsigned>;
//	{
//		Vec_t v0 = { 1 };
//		Vec_t v1 = { 2 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		std::cout << t0 << std::endl << t0.size() << std::endl << t1.size()
//				<< std::endl;
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size());
//	}
//	{
//		Vec_t v0 = { 1, 2, 3 };
//		Vec_t v1 = { 4, 5, 6 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		std::cout << t0 << std::endl;
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size());
//	}
//	{
//		Vec_t v0 = { 1, 2, 3 };
//		Vec_t v1 = { 4, 5, 6, 7, 8, 9, 10 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		std::cout << t0 << std::endl;
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size());
//	}
//	{
//		Vec_t v0 = { 3, 0 };
//		Vec_t v1 = { 5, 0 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		std::cout << t0 << std::endl;
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);
//	}
//	{
//		Vec_t v0 = { 20, 30, 35, 40, 50, 0 };
//		Vec_t v1 = { 5, 2, 9, 1, 6, 8, 0 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);
//
//	}
//	{
//		Vec_t v0 = { 2, 1, 3, 0, };
//		Vec_t v1 = { 8, 9, 4, 5, 0 };
//		Tree_t t0(v0);
//		Tree_t t1(v1);
//		t0.merge(t1);
//		BOOST_CHECK(t1.size() == 0);
//		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);
//
//	}
//}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
