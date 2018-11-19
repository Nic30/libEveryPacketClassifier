#include "TreeUtils.h"

#include <algorithm>

using namespace std;

namespace TreeUtils {

	Range1d RangeInBoundary1D(Range1d r, Range1d boundary) {
		auto low = max(r.low, boundary.low);
		auto high = min(r.high, boundary.high);
		return Range1d(low, high);
	}

	bool CompareRanges(const Range1d& a, const Range1d& b) {
		if (a.low != b.low) {
			return a.low < b.low;
		} else {
			return a.high < b.high;
		}
	}

	bool InsideOutCompare(const Range1d& a, const Range1d& b) {
		if (ContainsRange(a, b)) {
			return false;
		} else if (ContainsRange(b, a)) {
			return true;
		} else {
			return CompareRanges(a, b);
		}
	}

	bool CompareSecond(const pair<int, int> & x, const pair<int, int> & y) {
		return x.second > y.second;
	}

	bool AreEqual(const Rule& rule1, const Rule& rule2, const vector<Range1d>& boundary) {
		for (size_t i = 0; i < boundary.size(); i++) {
			Range1d r1 = RangeInBoundary1D(rule1.range[i], boundary[i]);
			Range1d r2 = RangeInBoundary1D(rule2.range[i], boundary[i]);
			if (r1.low > r2.low || r1.high < r2.high) {
				return false;
			}
		}
		return true;
	}

	bool AreSameRules(const list<Rule*>& c1, const list<Rule*>& c2) {
		if (c1.empty() || c2.empty()) {
			return false;
		}
		if (c1.size() != c2.size()) {
			return false;
		}


		size_t num = 0;
		for (const Rule* r1 : c1) {
			bool found = false;
			for (const Rule* r2 : c2) {
				if (r1->priority == r2->priority) {
					found = true;
					num++;
					break;
				}
			}
			if (!found) {
				return false;
			}
		}
		if (num != c1.size()) {
			printf("ERR: found the wrong number of matches.\n");
		}
		return true;
	}

	void RemoveRedund(list<Rule*> & rules, const vector<Range1d>& boundary) {
		list<Rule*> rulelist;
		for (Rule* rule : rules) {
			bool found = false;
			for (Rule* mule : rulelist) {
				if (AreEqual(*mule, *rule, boundary)) {
					found = true;
					break;
				}
			}
			if (!found) {
				rulelist.push_back(rule);
			}
		}
		rules.clear();
		rules = rulelist;
		rules.unique([](Rule* r1, Rule* r2) { return r1->priority == r2->priority; });
	}
}
