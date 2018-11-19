#pragma once

#include "BitVector.h"

struct EqnTerm {
	Point1d mask;
	Point1d xored;
	size_t boost;
};

class EqnMatcher : public FieldMatcher {
public:
	EqnMatcher(std::vector<Range1d>& ranges);
	~EqnMatcher();

	virtual size_t Match(Point1d x) const;

private:
	std::vector<EqnTerm> terms;
};

