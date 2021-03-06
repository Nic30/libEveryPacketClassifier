#include "BitVector.h"
#include "LongestPrefixMatch.h"
#include "EqnMatcher.h"

#include <set>

using namespace std;
using namespace TreeUtils;

int PrefixLength(Range1d s) {
	Point1d delta = s.high - s.low;
	int len = 0;
	while (delta) {
		delta >>= 1;
		len++;
	}
	return len;
}

BinaryRangeSearch::BinaryRangeSearch(vector<Range1d>& ranges) {
	set<Point1d> points;
	for (const Range1d & s : ranges) {
		points.insert(s.low);
		points.insert(s.high + 1);
	}
	points.erase(0);
	bool isFound = false;
	dividers.insert(dividers.end(), points.begin(), points.end());
	for (size_t j = 0; j < ranges.size(); j++) {
		if (ranges[j].contains(0)) {
			indices.push_back(j);
			isFound = true;
			break;
		}
	}
	if (!isFound) {
		indices.push_back(ranges.size());
	}
	for (size_t i = 0; i < dividers.size(); i++) {
		isFound = false;
		for (size_t j = 0; j < ranges.size(); j++) {
			if (ranges[j].contains(dividers[i])) {
				indices.push_back(j);
				isFound = true;
				break;
			}
		}
		if (!isFound) {
			indices.push_back(ranges.size());
		}
	}
}

size_t BinaryRangeSearch::Match(Point1d x) const {
	size_t index = Seek(x, 0, dividers.size());
	return indices[index];
}

size_t BinaryRangeSearch::Seek(Point1d x, size_t l, size_t r) const {
	//printf("%u %u\n", l, r);
	if (l == r) return l;
	size_t m = (l + r) / 2;
	if (x < dividers[m]) {
		return Seek(x, l, m);
	} else {
		return Seek(x, m + 1, r);
	}
}

// *********
// BitVector
// *********

BitVector::BitVector() {}


BitVector::~BitVector() {
	for (auto m : matchers) {
		delete m;
	}
}

void BitVector::_ConstructClassifier(const std::vector<Rule>& rules) {
	this->rules.insert(this->rules.end(), rules.begin(), rules.end());
	SortRules(this->rules);

	size_t dMax = 2;// this->rules[0].range.size();
	for (size_t d = 0; d < dMax; d++) {
		fields.push_back(vector<BitSet>());

		set<Range1d, RangeComp> ranges;
		for (const Rule& rule : this->rules) {
			ranges.insert(rule.range[d]);
		}

		vector<Range1d> rangeVector(ranges.begin(), ranges.end());
		//matchers.push_back(new BinaryRangeSearch(rangeVector));
		matchers.push_back(new LongestPrefixMatch(rangeVector));
		//matchers.push_back(new EqnMatcher(rangeVector));

		for (const Range1d& s : rangeVector) {
			BitSet matches(this->rules.size());
			for (size_t i = 0; i < this->rules.size(); i++) {
				if (ContainsRange(this->rules[i].range[d], s)) {
					matches.Set(i);
				}
			}
			fields[d].push_back(matches);
		}
	}
}

int BitVector::ClassifyAPacket(const Packet& packet) {
	BitSet sol(rules.size(), true);

	for (size_t i = 0; i < matchers.size(); i++) {
		size_t j = matchers[i]->Match(packet[i]);
		if (j < fields[i].size()) {
			sol &= fields[i][j];
			//fields[i][j].Print();
		} else {
			//printf("Early out\n");
			return -1;
		}
	}
	//sol.Print();

	for (size_t index = sol.FindFirst(); index < rules.size(); index = sol.FindNext(index)) {
		//printf("%u\n", index);
		if (rules[index].MatchesPacket(packet)) {
			//printf("Match!\n");
			return rules[index].priority;
		}
	}
	//printf("Failed\n");
	return -1;
}

// ***********
// BitVector64
// ***********

BitVector64::BitVector64() {}


BitVector64::~BitVector64() {
	for (auto m : matchers) {
		delete m;
	}
}

void BitVector64::_ConstructClassifier(const std::vector<Rule>& rules) {
	this->rules.insert(this->rules.end(), rules.begin(), rules.end());
	SortRules(this->rules);

	size_t dMax = 2;// this->rules[0].range.size();
	for (size_t d = 0; d < dMax; d++) {
		fields.push_back(vector<BitSet64>());

		set<Range1d, RangeComp> ranges;
		for (const Rule& rule : this->rules) {
			ranges.insert(rule.range[d]);
		}

		vector<Range1d> rangeVector(ranges.begin(), ranges.end());
		//matchers.push_back(new BinaryRangeSearch(rangeVector));
		//matchers.push_back(new LongestPrefixMatch(rangeVector));
		matchers.push_back(new EqnMatcher(rangeVector));

		for (const Range1d& s : rangeVector) {
			BitSet64 matches;
			for (size_t i = 0; i < this->rules.size(); i++) {
				if (ContainsRange(this->rules[i].range[d], s)) {
					matches.Set(i);
				}
			}
			fields[d].push_back(matches);
		}
	}
}

int BitVector64::ClassifyAPacket(const Packet& packet) {
	BitSet64 sol(true);

	for (size_t i = 0; i < matchers.size(); i++) {
		size_t j = matchers[i]->Match(packet[i]);
		if (j < fields[i].size()) {
			sol &= fields[i][j];
			//fields[i][j].Print();
		} else {
			//printf("Early out\n");
			return -1;
		}
	}
	//sol.Print();

	for (size_t index = sol.FindFirst(); index < rules.size(); index = sol.FindNext(index)) {
		//printf("%u\n", index);
		if (rules[index].MatchesPacket(packet)) {
			//printf("Match!\n");
			return rules[index].priority;
		}
	}
	//printf("Failed\n");
	return -1;
}
