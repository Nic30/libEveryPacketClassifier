#pragma once

#include "../ElementaryClasses.h"

namespace TreeUtils {

	Range1d RangeInBoundary1D(Range1d r, Range1d boundary);

	bool CompareRanges(const Range1d& a, const Range1d& b);

	bool InsideOutCompare(const Range1d& a, const Range1d& b);

	bool CompareSecond(const std::pair<int, int> & x, const std::pair<int, int> & y);

	bool AreEqual(const Rule& rule1, const Rule& rule2, const std::vector<Range1d>& boundary);

	bool AreSameRules(const std::list<Rule*>& c1, const std::list<Rule*>& c2);

	void RemoveRedund(std::list<Rule*> & rules, const std::vector<Range1d>& boundary);

	inline bool ContainsRange(const Range1d& outer, const Range1d& inner) {
		return outer.low <= inner.low && outer.high >= inner.high;
	}

	struct RangeComp {
		bool operator()(const Range1d& lhs, const Range1d& rhs) const { return InsideOutCompare(lhs, rhs); }
	};
}

