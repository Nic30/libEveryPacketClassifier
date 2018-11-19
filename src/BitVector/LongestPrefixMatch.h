#pragma once

#include "BitVector.h"

class LongestPrefixMatch : public FieldMatcher {
public:
	LongestPrefixMatch(std::vector<Range1d>& ranges);
	~LongestPrefixMatch();

	size_t Match(Point1d x) const;
private:
	std::map<int, std::unordered_map<Point1d, size_t>> table; // Length -> Low Bound -> Index
};

