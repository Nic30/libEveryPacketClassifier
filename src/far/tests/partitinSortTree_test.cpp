#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rbtree_test
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../partitionSortTree.h"

using Tree_t = PartitionSortTree<unsigned, unsigned, 2>;
BOOST_AUTO_TEST_SUITE( rbtree_testsuite )

BOOST_AUTO_TEST_CASE( simple_add_and_discard ) {
	Tree_t t({0, 1});
	unsigned N = 8;

	for (int r = 0; r < 1; r++) {
		N += r * N;
		// add some items
		for (unsigned i = 0; i < N; i++) {
			Tree_t::rule_type r = { Range1d(i, i), Range1d(i, i) };
			t.insert(r, i);
			BOOST_CHECK_EQUAL(t.size(), i + 1);
		}
		for (unsigned i = 0; i < N; i++) {
			Tree_t::value_type v = {i, i};
			BOOST_CHECK_EQUAL(t.find(v), i);
		}

		//std::ofstream myfile("simple_add_and_discard.dot");
		//myfile << t << std::endl;
		//myfile.close();

		// remove all items
		for (unsigned i = 0; i < N; i++) {
			BOOST_CHECK_EQUAL(t.size(), N - i);
			Tree_t::rule_type r = { Range1d(i, i), Range1d(i, i) };
			t.remove(r, i);
			BOOST_CHECK_EQUAL(t.size(), N - i - 1);

			Tree_t::value_type v = {i, i};
			BOOST_CHECK_EQUAL(t.find(v), Tree_t::RULE_NOT_FOUND);
		}

	}

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
