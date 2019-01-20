
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rule_resolver_test

#include <boost/test/unit_test.hpp>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>
#include <fstream>

#include "../fragmentedValue.h"
#include "../ruleGroupResolver.h"
#include "IO/InputReader.h"
#include "../llrbTree.h"
#include "../compressedDecisionTreeCompiler.h"
#include "../classificationRule.h"


using fv_t = ClassificationRule<uint16_t, 2>;
using v_t = fv_t::value_type;

BOOST_AUTO_TEST_SUITE( compressed_decision_tree_testsuite )


unsigned all = sizeof(uint16_t) * 8;
unsigned half = all / 2;
BOOST_AUTO_TEST_CASE( just3_exact_matches ) {
	using Compiler = CompressedDecisionTreeCompiler<fv_t>;
	std::vector<fv_t> rules = {
		fv_t({v_t(0, all), v_t(1, all)}, 1),
		fv_t({v_t(2, all), v_t(3, all)}, 2),
		fv_t({v_t(4, all), v_t(5, all)}, 3),
	};
	LLRBTree<fv_t> rbtree;
	rbtree.insert(rules);
	Compiler comp;
	Compiler::FieldOrder f_order = {0, 1};
	Compiler::Boundary r0 = { 0, 1 },
			r1 = { 2, 3 }, r2 = { 4, 5 },
			r3 = { 0, 0 }, r4 = { 2, 5 };

	std::ofstream f0("just3_exact_matches_llrb.dot");
	f0  << rbtree << std::endl;
	f0.close();

	auto c = comp.compile(rbtree, f_order);
	std::ofstream f1("just3_exact_matches.dot");
	f1  << *c << std::endl;
	f1.close();

	BOOST_CHECK_EQUAL(c->classify(r0), 1);
	BOOST_CHECK_EQUAL(c->classify(r1), 2);
	BOOST_CHECK_EQUAL(c->classify(r2), 3);
	auto inv = Compiler::ComprTree::INVALID_RULE;
	BOOST_CHECK_EQUAL(c->classify(r3), inv);
	BOOST_CHECK_EQUAL(c->classify(r4), inv);
}

//BOOST_AUTO_TEST_CASE( many_groups ) {
//	std::vector<fv_t> rules0 = {
//		fv_t({v_t(0, 0), v_t(1, all)}),
//		fv_t({v_t(0, 0), v_t(3, all)}),
//		fv_t({v_t(0, 0), v_t(5, all)}),
//	};
//	std::vector<fv_t> rules1 = {
//		fv_t({v_t(0, all), v_t(0, 0)}),
//		fv_t({v_t(2, all), v_t(0, 0)}),
//		fv_t({v_t(4, all), v_t(0, 0)}),
//	};
//	std::vector<fv_t> rules2 = {
//		fv_t({v_t(6, half), v_t(9, half)}),
//		fv_t({v_t(7, half), v_t(10, half)}),
//		fv_t({v_t(8, half), v_t(11, half)}),
//	};
//
//	RuleGroupResolver<fv_t> res;
//	res.add_rules(rules0);
//	res.add_rules(rules1);
//	BOOST_CHECK_EQUAL(res.rule_cnt, rules0.size() + rules1.size());
//	BOOST_CHECK_EQUAL(res.groups.size(), 2);
//
//	res.add_rules(rules2);
//	size_t size_sum1 = rules0.size() + rules1.size() +  rules2.size();
//	BOOST_CHECK_EQUAL(res.rule_cnt, size_sum1);
//	BOOST_CHECK_EQUAL(res.groups.size(), 3);
//}
//
//
//BOOST_AUTO_TEST_CASE( real_rules0 ) {
//	auto fn = "tests/rulesets/acl1_100";
//	//auto fn =  "../classbench-ng/generated/fw1_2000";
//	//auto fn =  "../classbench-ng/generated/acl1_10000";
//
//	int groups = 12;
//
//	auto _rules =  InputReader::ReadFilterFile(fn);
//	BOOST_CHECK_EQUAL(_rules.size(), 91);
//	using R = FragmentedValue<MaskedValue<unsigned>, 5>;
//	using V = fv_t::value_type;
//	std::vector<R> rules;
//	for (const auto & _r: _rules) {
//		auto _0 = _r.range[0];
//		auto _1 = _r.range[1];
//		auto _2 = _r.range[2];
//		auto _3 = _r.range[3];
//		auto _4 = _r.range[4];
//
//		R r({
//			V::from_range(_0.low, _0.high),
//			V::from_range(_1.low, _1.high),
//			V::from_range(_2.low, _2.high),
//			V::from_range(_3.low, _3.high),
//			V::from_range(_4.low, _4.high),
//		});
//		rules.push_back(r);
//	}
//	RuleGroupResolver<R> res;
//	res.add_rules(rules);
//	BOOST_CHECK_EQUAL(res.groups.size(), groups);
//
//	//for (const auto & item: res.groups) {
//	//	for (auto k: item.first )
//	//		std::cout << std::hex << k << ", ";
//	//	std::cout << "    " << std::dec << item.second.size() << std::endl;
//	//}
//}
//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE_END()

