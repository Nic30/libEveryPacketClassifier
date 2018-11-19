#include "LongestPrefixMatch.h"

using namespace std;

Point1d LPMMask(int len) {
	if (len == 0)
		return 0x0;
	else
		return numeric_limits<Point1d>::max() << (sizeof(Point1d) * 8 - len);
}

LongestPrefixMatch::LongestPrefixMatch(vector<Range1d>& ranges) {
	for (size_t i = 0; i < ranges.size(); i++) {
		int len = PrefixLength(ranges[i]);
		table[len][ranges[i].low] = len;
	}
}

LongestPrefixMatch::~LongestPrefixMatch() {
}

size_t LongestPrefixMatch::Match(Point1d x) const {
	for (auto iter = table.rbegin(); iter != table.rend(); iter++) {
		int len = iter->first;
		Point1d p = x & LPMMask(len);
		auto res = iter->second.find(p);
		if (res != iter->second.end()) {
			return res->second;
		}
	}
	return -1;
}
