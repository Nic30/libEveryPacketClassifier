#include "EqnMatcher.h"

using namespace std;

Point1d EqnMask(int len) {
	if (len == 0)
		return 0x0;
	else
		return numeric_limits<Point1d>::max() >> (sizeof(Point1d) * 8 - len);
}

EqnMatcher::EqnMatcher(vector<Range1d>& ranges) {
	vector<Range1d> reverse(ranges.rbegin(), ranges.rend());
	for (size_t i = 0; i < reverse.size(); i++) {
		int len = PrefixLength(reverse[i]);
		EqnTerm t;
		t.xored = reverse[i].low;
		t.mask = EqnMask(len);
		t.boost = i;
		for (size_t j = 0; j < i; j++) {
			if (TreeUtils::ContainsRange(reverse[j], reverse[i])) {
				t.boost -= terms[j].boost;
			}
		}
		terms.push_back(t);
	}
}

EqnMatcher::~EqnMatcher() {
}

size_t EqnMatcher::Match(Point1d x) const {
	size_t index = 0;
	for (EqnTerm t : terms) {
		//printf("%x %x: +%d\n", t.xor, t.mask, t.boost);
		//printf("%x\n", x ^ t.xor);
		index += ((x ^ t.xored) <= t.mask) * t.boost;
	}
	//printf("%u\n", index);
	//printf("\n");
	return terms.size() - index - 1;
}
