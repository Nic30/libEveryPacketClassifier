
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rule_resolver_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>
#include "../fragmentedValue.h"
#include "../ruleGroupResolver.h"
#include "IO/InputReader.h"


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
	BOOST_CHECK_EQUAL(res.rule_cnt, rules0.size() + rules1.size());
	BOOST_CHECK_EQUAL(res.groups.size(), 2);
	std::cout << res << std::endl;

	res.add_rules(rules2);
	size_t size_sum1 = rules0.size() + rules1.size() +  rules2.size();
	BOOST_CHECK_EQUAL(res.rule_cnt, size_sum1);
	BOOST_CHECK_EQUAL(res.groups.size(), 3);
}


BOOST_AUTO_TEST_CASE( real_rules0 ) {
	auto fn = "tests/rulesets/acl1_100";
	//auto fn =  "../classbench-ng/generated/fw1_2000";
	//auto fn =  "../classbench-ng/generated/acl1_10000";

	int groups = 12;

	auto _rules =  InputReader::ReadFilterFile(fn);
	BOOST_CHECK_EQUAL(_rules.size(), 91);
	using R = FragmentedValue<MaskedValue<unsigned>, 5>;
	using V = fv_t::value_type;
	std::vector<R> rules;
	for (const auto & _r: _rules) {
		auto _0 = _r.range[0];
		auto _1 = _r.range[1];
		auto _2 = _r.range[2];
		auto _3 = _r.range[3];
		auto _4 = _r.range[4];

		R r({
			V::from_range(_0.low, _0.high),
			V::from_range(_1.low, _1.high),
			V::from_range(_2.low, _2.high),
			V::from_range(_3.low, _3.high),
			V::from_range(_4.low, _4.high),
		});
		rules.push_back(r);
	}
	RuleGroupResolver<R> res;
	res.add_rules(rules);
	BOOST_CHECK_EQUAL(res.groups.size(), groups);

	//for (const auto & item: res.groups) {
	//	for (auto k: item.first )
	//		std::cout << std::hex << k << ", ";
	//	std::cout << "    " << std::dec << item.second.size() << std::endl;
	//}
}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()

