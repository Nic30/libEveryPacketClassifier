#include "HyperSplit_nodes.h"
using namespace std;

#define NodeSize 8


void CheckBounds(const vector<Range1d> &bounds, const string &s) {
	if (bounds.size() != 5) {
		throw std::runtime_error(string("Bad size! ") + string(s)
				+ " Incorrect bounds: " + to_string(bounds.size()));
	}
}

void CheckBounds(const vector<Range1d> &bounds) {
	string s = "";
	CheckBounds(bounds, s);
}

void PrintBounds(const vector<Range1d> &bounds) {
	cout << "(";
	for (const auto& s : bounds) {
		cout << s << ",";
	}
	cout << ")" << endl;
}

vector<Rule> LeftList(const vector<Rule>& rules, int dim, unsigned int splitPoint) {
	vector<Rule> leftList;
	for (const Rule& r : rules) {
		if (r.range[dim].low <= splitPoint) {
			leftList.push_back(r);
		}
	}
	return leftList;
}

vector<Rule> RightList(const vector<Rule>& rules, int dim, unsigned int splitPoint) {
	vector<Rule> rightList;
	for (const Rule& r : rules) {
		if (r.range[dim].high > splitPoint) {
			rightList.push_back(r);
		}
	}
	return rightList;
}

vector<Point1d> GetSplitPoints(const vector<Range1d>& bounds, const vector<Rule>& rules, int dim) {
	set<Point1d> lows, highs;
	lows.insert(bounds[dim].low);
	highs.insert(bounds[dim].high);

	for (const Rule& r : rules) {
		lows.insert(max(r.range[dim].low, bounds[dim].low));
		highs.insert(min(r.range[dim].high, bounds[dim].high));
	}
	lows.erase(lows.find(bounds[dim].low));
	for (Point1d l : lows) {
		highs.insert(l - 1);
	}

	vector<Point1d> result;
	for (Point1d h : highs) {
		result.push_back(h);
	}
	sort(result.begin(), result.end());
	return result;
}

vector<Range1d> GetRanges(const vector<Range1d>& bounds, const vector<Rule>& rules, int dim) {
	auto splits = GetSplitPoints(bounds, rules, dim);
	vector<Range1d> results;
	Point1d l = bounds[dim].low;
	for (Point1d r : splits) {
		results.push_back(Range1d(l, r));
		l = r + 1;
	}
	return results;
}

unsigned int SelectDimToSplit(const vector<Range1d>& bounds, const vector<Rule>& rules) {
	auto dimCost = [&rules, &bounds](int dim) -> double {
		auto segments = GetRanges(bounds, rules, dim);

		vector<int> costs;
		for (Range1d s : segments) {
			int c = count_if(rules.begin(), rules.end(), [=](const Rule& r) -> bool {
				return r.range[dim].low <= s.low && r.range[dim].high >= s.high; });
			costs.push_back(c);
		}

		int sum = 0;
		for (int c : costs) {
			sum += c;
		}
		return ((double)sum) / costs.size();
	};
	vector<double> costs;
	for (int dim = 0; dim < rules[0].dim; dim++) {
		costs.push_back(dimCost(dim));
	}
	double minCost = *min_element(costs.begin(), costs.end());
	for (int dim = 0; dim < rules[0].dim; dim++) {
		if (costs[dim] == minCost) return dim;
	}
	return -1;
}

Point1d SelectSplitPoint(const vector<Range1d>& bounds, const vector<Rule>& rules, int dim) {
	vector<Range1d> segments = GetRanges(bounds, rules, dim);
	vector<int> costs;

	for (Range1d s : segments) {
		int c = count_if(rules.begin(), rules.end(), [=](const Rule& r) -> bool {
			return r.range[dim].low <= s.low && r.range[dim].high >= s.high; });
		costs.push_back(c);
	}
	int sum = 0;
	for (int c : costs) {
		sum += c;
	}
	int partSum = 0;
	size_t i;
	for (i = 0; i < costs.size(); i++) {
		partSum += costs[i];
		if (partSum >= sum / 2) {
			break;
		}
	}

	if (segments.size() == 1) {
		return segments[i].high;
	} else if (i + 1 == segments.size()) {
		return segments[i - 1].high;
	} else {
		return segments[i].high;
	}
}

iNode* SplitRules(const vector<Range1d>& bounds, const vector<Rule>& rules, unsigned int leafSize) {
	//PrintBounds(bounds);
	//CheckBounds(bounds);

	if (rules.size() <= leafSize) {
		return new ListNode(bounds, rules);
	} else {
		unsigned int dim = SelectDimToSplit(bounds, rules);
		Point1d split = SelectSplitPoint(bounds, rules, dim);

		if (dim >= bounds.size()) {
			throw std::runtime_error("Bad dim!");
		}

		if (split != bounds[dim].high) {
			vector<Range1d> lbounds(bounds), rbounds(bounds);
			lbounds[dim].high = split;
			rbounds[dim].high = split + 1;
			auto lrl = LeftList(rules, dim, split);
			auto rrl = RightList(rules, dim, split);

			if (lrl.size() == rules.size() || rrl.size() == rules.size()) {
				// Won't divide anymore
				return new ListNode(bounds, rules);
			} else {
				auto lc = SplitRules(lbounds, lrl, leafSize);
				auto rc = SplitRules(rbounds, rrl, leafSize);
				return new SplitNode(bounds, split, dim, lc, rc);
			}
		} else {
			return new ListNode(bounds, rules);
		}
	}
}



// **********
// Split Node
// **********

int SplitNode::ClassifyAPacket(const Packet& p) {
	unsigned int pt = p[splitDim];
	if (pt <= splitPoint) {
		return leftChild->ClassifyAPacket(p);
	} else {
		return rightChild->ClassifyAPacket(p);
	}
}

iNode* SplitNode::DeleteRule(const Rule& r) {
	//CheckBounds(bounds);

	unsigned int low = r.range[splitDim].low;
	unsigned int high = r.range[splitDim].high;

	if (low <= splitPoint) {
		iNode* lc = leftChild->DeleteRule(r);
		if (lc != leftChild) {
			delete leftChild;
			leftChild = lc;
		}
	}
	if (high > splitPoint) {
		iNode* rc = rightChild->DeleteRule(r);
		if (rc != rightChild) {
			delete rightChild;
			rightChild = rc;
		}
	}

	if (leftChild->IsEmpty()) {
		iNode* res = rightChild;
		rightChild = nullptr;
		res->SetBounds(bounds);
		return res;
	} else if (rightChild->IsEmpty()) {
		iNode* res = leftChild;
		leftChild = nullptr;
		res->SetBounds(bounds);
		return res;
	} else {
		return this;
	}
}

iNode* SplitNode::InsertRule(unsigned int leafSize, const Rule& one_rule) {
	//CheckBounds(bounds);
	unsigned int low = one_rule.range[splitDim].low;
	unsigned int high = one_rule.range[splitDim].high;

	if (low <= splitPoint) {
		iNode* lc = leftChild->InsertRule(leafSize, one_rule);
		if (lc != leftChild) {
			delete leftChild;
			leftChild = lc;
		}
	}
	if (high > splitPoint) {
		iNode* rc = rightChild->InsertRule(leafSize, one_rule);
		if (rc != rightChild) {
			delete rightChild;
			rightChild = rc;
		}
	}

	return this;
}

int SplitNode::Size(int ruleSize) const {
	return NodeSize + leftChild->Size(ruleSize) + rightChild->Size(ruleSize);
}

void SplitNode::SetBounds(const vector<Range1d>& bounds) {
	CheckBounds(bounds);
	this->bounds = bounds;
}

// ********
// ListNode
// ********

int ListNode::ClassifyAPacket(const Packet& p) {
	int bestPriority = -1;
	for (size_t i = 0; i < rules.size(); i++) {
		if (rules[i].priority > bestPriority) {
			bool matches = true;
			for (int d = 0; d < rules[i].dim; d++) {
				if (!(rules[i].range[d].low <= p[d] && rules[i].range[d].high >= p[d])) {
					matches = false;
					break;
				}
			}
			if (matches) {
				bestPriority = rules[i].priority;
			}
		}
	}
	return bestPriority;
}

iNode* ListNode::DeleteRule(const Rule& r) {
	//CheckBounds(bounds);
	rules.erase(remove_if(rules.begin(), rules.end(), [=](const Rule& r2) -> bool {return r.priority == r2.priority; }), rules.end());
	return this;
}

iNode* ListNode::InsertRule(unsigned int leafSize, const Rule& r) {
	// CheckBounds(bounds);
	rules.push_back(r);
	if (rules.size() <= leafSize) {
		return this;
	} else {
		iNode* n = SplitRules(bounds, rules, leafSize);
		return n;
	}
}

int ListNode::Size(int ruleSize) const {
	return NodeSize + ruleSize * rules.size();
}

void ListNode::SetBounds(const vector<Range1d>& bounds) {
	CheckBounds(bounds);
	this->bounds = bounds;
}

