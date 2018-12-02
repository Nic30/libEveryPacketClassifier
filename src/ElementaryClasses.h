#pragma once

#include <vector>
#include <queue>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <random>
#include <numeric>
#include <memory>
#include <chrono> 
#include <array>
#include <sstream>

enum Dimensions {
	FieldSA = 0, FieldDA = 1, FieldSP = 2, FieldDP = 3, FieldProto = 4,
};

using Point1d = uint32_t;

template<size_t dimension_cnt>
class PointNd: public std::array<Point1d, dimension_cnt> {
};

using Packet = std::vector<Point1d>;

class Range1d {
public:
	Point1d low;
	Point1d high;

	Range1d(): low(0), high(0) {}
	Range1d(Point1d low, Point1d high) :
			low(low), high(high) {
	}

	inline bool ContainsPoint(Point1d x) {
		return low <= x && x <= high;
	}
	bool operator <(const Range1d& other) const {
		if (low != other.low) {
			return low < other.low;
		} else
			return high < other.high;
	}

	bool operator >(const Range1d& other) const {
		if (high != other.high) {
			return high > other.high;
		} else
			return low > other.low;
	}

	bool isIntersect(const Range1d & other) const {
		return std::max(low, other.low) <= std::min(high, other.high);
	}

	bool operator ==(const Range1d& other) const {
		return low == other.low && high == other.high;
	}

	friend std::ostream & operator<<(std::ostream & str, const Range1d & r) {
		str << "[" << r.low << "-" << r.high << "]";
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};

/**
 * Filter rule
 *
 * @ivar dim number of dimensions in rule
 * @ivar priority priority of rule in ruleset
 *
 * @ivar id id number used by PartitoinSort
 * @ivar tag tag number used by PartitionSort
 * @ivar markedDelete flag which signalize that this rule was deleted, used by PartitionSort
 *
 * @ivar range ranges for each dimension
 * @ivar prefix_length length of prefix of range for all dimensions
 * */
struct Rule {
	Rule(int dim = 5) :
			dim(dim), priority(-1), id(-1), tag(-1), range(dim, { 0, 0 }),
			prefix_length(dim, 0) {
		markedDelete = 0;
	}

	int dim;
	int priority;

	/// PartitionSort specifics [TODO] mv to PartitionSort implementation
	int id;
	int tag;
	bool markedDelete = false;

	std::vector<Range1d> range;
	std::vector<unsigned> prefix_length;

	bool inline MatchesPacket(const Packet& p) const {
		for (int i = 0; i < dim; i++) {
			if (p[i] < range[i].low || p[i] > range[i].high)
				return false;
		}
		return true;
	}

	friend std::ostream & operator<<(std::ostream & str, const Rule & r) {
		for (const auto& _r: r.range) {
			str << _r << '\t';
		}
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};

class Range1dWeighted: public Range1d {
public:

	Range1dWeighted(): Range1dWeighted(0, 0, -1) {}
	Range1dWeighted(Point1d low, Point1d high, int id) :
			Range1d(low, high), id(id), weight(1) {
	}

	int id;
	int weight;
};

class EndPoint {
public:
	EndPoint(double val, bool isRightEnd, int id) :
			val(val), isRightEnd(isRightEnd), id(id) {}

	bool operator <(const EndPoint & rhs) const {
		return val < rhs.val;
	}

	double val;
	bool isRightEnd;
	int id;
};

class Random {
public:
	// random number generator from Stroustrup: 
	// http://www.stroustrup.com/C++11FAQ.html#std-random
	// static: there is only one initialization (and therefore seed).
	static int random_int(int low, int high) {
		//static std::mt19937  generator;
		using Dist = std::uniform_int_distribution < int >;
		static Dist uid { };
		return uid(generator, Dist::param_type { low, high });
	}

	// random number generator from Stroustrup: 
	// http://www.stroustrup.com/C++11FAQ.html#std-random
	// static: there is only one initialization (and therefore seed).
	static int random_unsigned_int() {
		//static std::mt19937  generator;
		using Dist = std::uniform_int_distribution < unsigned int >;
		static Dist uid { };
		return uid(generator, Dist::param_type { 0, 4294967295 });
	}
	static double random_real_btw_0_1() {
		//static std::mt19937  generator;
		using Dist = std::uniform_real_distribution < double >;
		static Dist uid { };
		return uid(generator, Dist::param_type { 0, 1 });
	}

	template<class T>
	static std::vector<T> shuffle_vector(std::vector<T> vi) {
		//static std::mt19937  generator;
		std::shuffle(std::begin(vi), std::end(vi), generator);
		return vi;
	}
private:
	static std::mt19937 generator;
};

enum PSMode {
	NoCompression, PathCompression, PriorityNode, NoIntermediateTree
};

inline void SortRules(std::vector<Rule>& rules) {
	sort(rules.begin(), rules.end(),
			[](const Rule& rx, const Rule& ry) {
		return rx.priority >= ry.priority;
	});
}

inline void SortRules(std::vector<Rule*>& rules) {
	sort(rules.begin(), rules.end(),
			[](const Rule* rx, const Rule* ry) {
		return rx->priority >= ry->priority;
	});
}
