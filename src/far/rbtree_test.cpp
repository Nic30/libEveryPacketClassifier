#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rbtree_test
#include <boost/test/unit_test.hpp>
#include <vector>
#include "RBTree.h"
#include "fragmented_value.h"

// https://www.boost.org/doc/libs/1_66_0/doc/html/intrusive/set_multiset.html
// https://github.com/boostorg/intrusive/blob/develop/example/doc_rbtree_algorithms.cpp
//____________________________________________________________________________//

// automatically registered test cases could be organized in test suites
BOOST_AUTO_TEST_SUITE( rbtree_testsuite )

BOOST_AUTO_TEST_CASE( simple_add_and_discard ) {
	RBTree<unsigned> t;

	// add some items
	for (unsigned i = 0; i < 10; i++) {
		t.add(i);
		BOOST_CHECK(t.size() == i + 1);
	}

	// discard items which are not present in tree
	for (unsigned i = 10; i < 20; i++) {
		bool was_discarded = t.discard(i);
		BOOST_CHECK(!was_discarded);
		BOOST_CHECK(t.size() == 10);
	}

	// remove all items
	for (unsigned i = 0; i < 10; i++) {
		BOOST_CHECK(t.size() == 10 - i);
		bool was_discarded = t.discard(i);
		BOOST_CHECK(was_discarded);
	}

}

BOOST_AUTO_TEST_CASE( add_and_discard_rev_order ) {
	RBTree<unsigned> t;
	// add some items
	for (unsigned i = 10; i < 20; i++) {
		t.add(i);
		BOOST_CHECK(t.size() == i - 10 + 1);
	}

	// discard items which are not present in tree
	for (unsigned i = 0; i < 10; i++) {
		bool was_discarded = t.discard(i);
		BOOST_CHECK(!was_discarded);
		BOOST_CHECK(t.size() == 10);
	}

	// remove all items in reversed order
	for (unsigned i = 0; i < 10; i++) {
		BOOST_CHECK(t.size() == 10 - i);
		bool was_discarded = t.discard(20 - i - 1);
		BOOST_CHECK(was_discarded);
	}
}

BOOST_AUTO_TEST_CASE( add_duplicit ) {
	using Tree_t = RBTree<unsigned>;
	Tree_t t;
	// add some items
	for (unsigned i = 0; i < 10; i++) {
		auto add = t.add(i);
		BOOST_CHECK(t.size() == i + 1);
		BOOST_CHECK(add);
	}
	for (unsigned i = 0; i < 10; i++) {
		auto add = t.add(i);
		BOOST_CHECK(t.size() == 10);
		BOOST_CHECK(!add);
	}

	// discard items which are not present in tree
	for (unsigned i = 0; i < 10; i++) {
		bool was_discarded = t.discard(i);
		BOOST_CHECK(was_discarded);
		BOOST_CHECK(t.size() == 10 - i - 1);
	}
}

BOOST_AUTO_TEST_CASE( merge ) {

	using Tree_t = RBTree<unsigned>;
	using Vec_t = std::vector<unsigned>;
	{
		Vec_t v0 = { 1 };
		Vec_t v1 = { 2 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		std::cout << t0 << std::endl << t0.size() << std::endl<< t1.size() << std::endl;
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size());
	}
	{
		Vec_t v0 = { 1, 2, 3 };
		Vec_t v1 = { 4, 5, 6 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		std::cout << t0 << std::endl;
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size());
	}
	{
		Vec_t v0 = { 1, 2, 3 };
		Vec_t v1 = { 4, 5, 6, 7, 8, 9, 10 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		std::cout << t0 << std::endl;
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size());
	}
	{
		Vec_t v0 = { 3, 0 };
		Vec_t v1 = { 5, 0 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		std::cout << t0 << std::endl;
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);
	}
	{
		Vec_t v0 = { 20, 30, 35, 40, 50, 0 };
		Vec_t v1 = { 5, 2, 9, 1, 6, 8, 0 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);

	}
	{
		Vec_t v0 = { 2, 1, 3, 0, };
		Vec_t v1 = { 8, 9, 4, 5, 0 };
		Tree_t t0(v0);
		Tree_t t1(v1);
		t0.merge(t1);
		BOOST_CHECK(t1.size() == 0);
		BOOST_CHECK(t0.size() == v0.size() + v1.size() - 1);

	}
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
