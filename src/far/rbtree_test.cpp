#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE rbtree_test
#include <boost/test/unit_test.hpp>
#include <functional>
//#include "RBTree.h"
#include "fragmented_value.h"
#include <boost/geometry.hpp>
#include <boost/intrusive/options.hpp>
//#include <boost/geometry/geometries/point.hpp>
//#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/register/point.hpp>
// https://www.boost.org/doc/libs/1_66_0/doc/html/intrusive/set_multiset.html
// https://github.com/boostorg/intrusive/blob/develop/example/doc_rbtree_algorithms.cpp
// https://stackoverflow.com/questions/42182537/how-can-i-use-the-rtree-of-the-boost-library-in-c
//____________________________________________________________________________//
using namespace boost::intrusive;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
//using point_t = bg::model::point<float, 2, bg::cs::cartesian>;
//using point_t = unsigned;
//using box = bg::model::box<point_t>;
//using value_t = std::pair<box, unsigned int>;
//
//enum Color {
//	RED, BLACK, DOUBLE_BLACK
//};
//
//template<typename KEY_t>
//class RBNode: set_base_hook<> {
//	using MyClass = RBNode<KEY_t>;
//public:
//
//	RBNode *left, *right, *parent;
//	Color color;
//	KEY_t key;
//	set_member_hook<> member_hook_;
//	explicit RBNode(KEY_t);
//
//	friend bool operator<(const MyClass &a, const MyClass &b) {
//		return a.key < b.key;
//	}
//	friend bool operator>(const MyClass &a, const MyClass &b) {
//		return a.key > b.key;
//	}
//	friend bool operator==(const MyClass &a, const MyClass &b) {
//		return a.key == b.key;
//	}
//
//};
//using Node_t = RBNode<unsigned>;
//using NodeMemberOption = member_hook<Node_t, set_member_hook<>, &Node_t::member_hook_>;
////using namespace boost::intrusive;
////using Node_t = RBNode<unsigned>;
////using Tree_t = rbtree<Node_t, Node_t::MemberOption>;
//using Tree_t = bgi::rtree<RBNode<unsigned>, bgi::quadratic<16> >;
//
BOOST_AUTO_TEST_SUITE( rbtree_testsuite )
//
BOOST_AUTO_TEST_CASE( simple_add_and_discard ) {
	Tree_t t;

	// add some items
	for (unsigned i = 0; i < 10; i++) {
		t.insert(i);
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
//
//BOOST_AUTO_TEST_CASE( add_and_discard_rev_order ) {
//	Tree_t t;
//	// add some items
//	for (unsigned i = 10; i < 20; i++) {
//		t.add(i);
//		BOOST_CHECK(t.size() == i - 10 + 1);
//	}
//
//	// discard items which are not present in tree
//	for (unsigned i = 0; i < 10; i++) {
//		bool was_discarded = t.discard(i);
//		BOOST_CHECK(!was_discarded);
//		BOOST_CHECK(t.size() == 10);
//	}
//
//	// remove all items in reversed order
//	for (unsigned i = 0; i < 10; i++) {
//		BOOST_CHECK(t.size() == 10 - i);
//		bool was_discarded = t.discard(20 - i - 1);
//		BOOST_CHECK(was_discarded);
//	}
//}
//
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
