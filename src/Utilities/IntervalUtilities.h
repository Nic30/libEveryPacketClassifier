#ifndef  UTIL_H
#define  UTIL_H
#include "../ElementaryClasses.h"


class WeightedInterval;
struct LightWeightedInterval;
class Utilities {
public:
	static double EPS;
	static bool  IsIdentical(const Rule& r1, const Rule& r2);
	static int GetMaxOverlap(const std::multiset<Point1d>& lo, const std::multiset<Point1d>& hi);
	static void _MWISIntervals_endpoints(std::vector<EndPoint> & endpoints);
	static std::pair<std::vector<int>, int> MWISIntervals(const std::vector<WeightedInterval>&I);
	static std::pair<std::vector<int>, int> FastMWISIntervals(const std::vector<LightWeightedInterval>&I);
	static std::pair<std::vector<int>, int> MWISIntervals(const std::vector<Rule>& I, int x);

	static std::vector<WeightedInterval> CreateUniqueInterval(const std::vector<Rule>& rules, int field);
	static std::vector<LightWeightedInterval> FastCreateUniqueInterval(const std::vector<Range1dWeighted>& rules);
	static std::vector<std::vector<WeightedInterval>> CreateUniqueIntervalsForEachField(const std::vector<Rule>& rules);
 
	static std::vector<Rule> RedundancyRemoval(const std::vector<Rule>& rules);

};
 
class LightWeightedInterval: public Range1d {
public:
	LightWeightedInterval(Point1d low, Point1d high, int w) : Range1d(low, high), weight(w) {}
	LightWeightedInterval(Range1d range, int w) : Range1d(range), weight(w) {}

	int weight;

	void Push(int x) {
		rule_indices.push_back(x);
	}
	std::vector<int> GetRuleIndices() const {
		std::cout << rule_indices.size() << std::endl;
		return rule_indices;
	}
private:
	std::vector<int> rule_indices;
};


class WeightedInterval: public Range1d {
public:

	WeightedInterval(const std::vector<Rule>& rules, Point1d low, Point1d high) : Range1d(low, high), rules(rules) {
		if (rules.size() == 0) {
			throw std::runtime_error("ERROR: EMPTY RULE AND CONSTRUCT INTERVAL?");
		}
		SetWeightBySizePlusOne();
		field = 0;
	}

	WeightedInterval(const std::vector<Rule>& rules, int field) : rules(rules), field(field) {
		if (rules.size() == 0) {
			throw std::runtime_error("ERROR: EMPTY RULE AND CONSTRUCT INTERVAL?");
		}
		low = rules[0].range[field].low;
		high = rules[0].range[field].high;
		if (low > high)
			throw std::runtime_error("WeightedInterval bad values for constructor");
		SetWeightBySizePlusOne();
	}

	int CountPOM(int second_field) {
		std::multiset<Point1d> Astart;
		std::multiset<Point1d> Aend;
		for (auto r: rules) {
			Astart.insert(r.range[second_field].low);
			Aend.insert(r.range[second_field].high);
		}
		return Utilities::GetMaxOverlap(Astart, Aend);
	}

	void SetWeightByPenaltyPOM(int second_field) {
		weight = std::max((int)rules.size() - 100 * CountPOM(second_field), 1);
	}

	void SetWeightBySizePlusOne() {
		weight = rules.size() +1;
	}

	void SetWeightBySize() {
		weight = rules.size();
	}

	std::vector<Rule> GetRules() const{ return rules; }
	int GetField() const { return field; }
	int GetWeight() const { return weight; }
protected:
	int weight = 100000;
	std::vector<Rule> rules;
	int  field;
};


#endif
