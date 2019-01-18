#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rule_resolver_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>
#include "fragmented_value.h"
#include "rule_group_resolver.h"


using fv_t = FragmentedValue<MaskedValue<unsigned>, 2>;
using v_t = fv_t::value_type;

BOOST_AUTO_TEST_SUITE( rule_group_resolver_testsuite )


unsigned all = sizeof(unsigned) * 8;
unsigned half = all / 2;
BOOST_AUTO_TEST_CASE( many_per_group ) {
	std::vector<fv_t> rules = {
		fv_t({v_t(0, all), v_t(1, all)}),
		fv_t({v_t(2, all), v_t(3, all)}),
		fv_t({v_t(4, all), v_t(5, all)}),
	};
	RuleGroupResolver<fv_t> res;
	res.add_rules(rules);
	BOOST_CHECK(res.rule_cnt == rules.size());
	BOOST_CHECK(res.groups.size() == 1);
}

BOOST_AUTO_TEST_CASE( many_groups ) {
	std::vector<fv_t> rules0 = {
		fv_t({v_t(0, 0), v_t(1, all)}),
		fv_t({v_t(0, 0), v_t(3, all)}),
		fv_t({v_t(0, 0), v_t(5, all)}),
	};
	std::vector<fv_t> rules1 = {
		fv_t({v_t(0, all), v_t(0, 0)}),
		fv_t({v_t(2, all), v_t(0, 0)}),
		fv_t({v_t(4, all), v_t(0, 0)}),
	};
	std::vector<fv_t> rules2 = {
		fv_t({v_t(6, half), v_t(9, half)}),
		fv_t({v_t(7, half), v_t(10, half)}),
		fv_t({v_t(8, half), v_t(11, half)}),
	};

	RuleGroupResolver<fv_t> res;
	res.add_rules(rules0);
	res.add_rules(rules1);
	BOOST_CHECK(res.rule_cnt == rules0.size() + rules1.size());
	BOOST_CHECK(res.groups.size() == 2);

	res.add_rules(rules2);
	BOOST_CHECK(res.rule_cnt == rules0.size() + rules1.size() +  rules2.size());
	BOOST_CHECK(res.groups.size() == 3);
}



//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()

