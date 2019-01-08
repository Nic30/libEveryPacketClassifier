//#include "fragmented_value.h"
//#include "rule_group_resolver.h"
//
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rule_resolver_test
//
#include <boost/test/unit_test.hpp>
#include <functional>
#include <vector>
#include <limits>
//
//
//using fv_t = FragmentedValue<MaskedValue<unsigned>, 2>;
//using v_t = fv_t::value_type;
//
// automatically registered test cases could be organized in test suites
BOOST_AUTO_TEST_SUITE( rule_group_resolver_testsuite )
//
//
//BOOST_AUTO_TEST_CASE( many_per_group ) {
//	auto all = std::numeric_limits<unsigned>::max();
//	std::vector<fv_t> rules = {
//		{v_t(0, all), v_t(1, all)},
//		{v_t(2, all), v_t(3, all)},
//		{v_t(4, all), v_t(5, all)},
//	};
//	RuleGroupResolver<fv_t> res;
//	res.add_rules(rules);
//	BOOST_CHECK(res.rule_cnt == rules.size());
//	BOOST_CHECK(res.groups.size() == 1);
//}
//
//
////____________________________________________________________________________//
//
BOOST_AUTO_TEST_SUITE_END()

