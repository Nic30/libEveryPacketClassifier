#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE fragmented_value_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>
#include "fragmented_value.h"

using fv_t = FragmentedValue<MaskedValue<unsigned>, 2>;
using v_t = fv_t::value_type;

// automatically registered test cases could be organized in test suites
BOOST_AUTO_TEST_SUITE( fragmented_value_testsuite )


BOOST_AUTO_TEST_CASE( constructors_and_compares ) {
	size_t all = sizeof(unsigned) * 8;
	fv_t val0 = fv_t({v_t(0, all), v_t(1, all)});
	BOOST_CHECK(val0[0].low() == 0);
	BOOST_CHECK(val0[0].high() == 0);
	BOOST_CHECK(val0[0].prefix_len == all);

	BOOST_CHECK(val0[1].low() == 1);
	BOOST_CHECK(val0[1].high() == 1);
	BOOST_CHECK(val0[1].prefix_len == all);

	std::vector<fv_t> rules = {
		fv_t({v_t(0, all), v_t(1, all)}),
		fv_t({v_t(2, all), v_t(3, all)}),
		fv_t({v_t(4, all), v_t(5, all)}),
	};

	for (auto& v: rules) {
		BOOST_CHECK(val0 <= v);
	}

	fv_t val1 = fv_t({v_t(0, 8), v_t(1, 8)});
	fv_t val2 = fv_t({v_t(0, 16), v_t(1, 16)});
	BOOST_CHECK_THROW(val1 <= val2, NonComparableErr);
	BOOST_CHECK(val1 == val1);
	BOOST_CHECK(val1 <= val1);
	BOOST_CHECK_THROW(val1 < val1, NonComparableErr);

}

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()
