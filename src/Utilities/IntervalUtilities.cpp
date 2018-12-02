#include "IntervalUtilities.h"

double Utilities::EPS = 0.0001;

std::vector<Rule> Utilities::RedundancyRemoval(const std::vector<Rule>& rules) {
	std::vector<bool> isVisited(rules.size(), 0);
	std::vector<Rule> out;
	for (size_t i = 0; i < rules.size(); i++) {
		if (!isVisited[i]) {
			isVisited[i] = true;
			out.push_back(rules[i]);
			for (size_t j = i + 1; j < rules.size(); j++) {
				if (IsIdentical(rules[i], rules[j])) {
					isVisited[j] = true;
				}
			}
		}
	}
	return out;
}

bool Utilities::IsIdentical(const Rule& r1, const Rule& r2) {
	for (int i = 0; i < r1.dim; i++) {
		if (r1.range[i].low != r2.range[i].low)
			return false;
		if (r1.range[i].high != r2.range[i].high)
			return false;
	}
	return true;
}

int Utilities::GetMaxOverlap(const std::multiset<unsigned int>& lo,
		const std::multiset<unsigned int>& hi) {
	if (lo.size() == 0) {
		return 0;
		//	cout << "error in GetMaxOverLap: lo size = 0 " << endl;
		//	exit(1);
	}
	std::vector<unsigned> start_time, end_time;
	for (const auto element : lo) {
		start_time.push_back(element);
	}
	for (const auto element : hi) {
		end_time.push_back(element);
	}
	size_t i = 0, j = 0;
	int max_overlap = -1;
	int current_overlap = 0;
	while (i < start_time.size() && j < end_time.size()) {
		if (start_time[i] <= end_time[j]) {
			current_overlap++;
			max_overlap = std::max(current_overlap, max_overlap);
			i++;
		} else {
			current_overlap--;
			j++;
		}
	}
	return max_overlap;
}

std::pair<std::vector<int>, int> Utilities::FastMWISIntervals(
		const std::vector<LightWeightedInterval>&I) {
	//I is already sorted 
	int temp_max = 0;
	int max_MIS = 0;
	std::vector<int> S_max;
	S_max.reserve(I.size());
	int last_interval = 0;
	std::vector<int> chi(I.size(), 0);

	std::vector<EndPoint> endpoints;
	for (size_t i = 0; i < I.size(); i++) {
		endpoints.push_back(EndPoint(I[i].low, 0, i));
		endpoints.push_back(EndPoint(I[i].high, 1, i));
	}

	_MWISIntervals_endpoints(endpoints);

	for (const auto& e : endpoints) {
		if (!e.isRightEnd) {
			chi[e.id] = temp_max + I[e.id].weight;
		} else {
			if (chi[e.id] > temp_max) {
				temp_max = chi[e.id];
				last_interval = e.id;
			}
		}
	}

	S_max.push_back(last_interval);
	max_MIS = temp_max;
	temp_max = temp_max - I[last_interval].weight;
	for (int j = last_interval - 1; j >= 0; j--) {
		if (chi[j] == temp_max && I[j].high < I[last_interval].low) {
			S_max.push_back(j);
			temp_max = temp_max - I[j].weight;
			last_interval = j;
		}
	}
	return std::make_pair(S_max, max_MIS);
}

std::pair<std::vector<int>, int> Utilities::MWISIntervals(
		const std::vector<Rule>& I, int x) {
	// create an weighted interval from rules
	std::vector<WeightedInterval> vc;
	for_each(begin(I), end(I),
			[&vc, x](const Rule & r) {
		vc.push_back(WeightedInterval(std::vector<Rule>(1, r), x));
	});
	return MWISIntervals(vc);
}

std::vector<LightWeightedInterval> Utilities::FastCreateUniqueInterval(
		const std::vector<Range1dWeighted>& rules_given_field) {

	size_t num_rules = rules_given_field.size();
	std::vector<LightWeightedInterval> out;
	if (num_rules == 0)
		return out;

	int count = 1;
	out.reserve(num_rules);
	Range1dWeighted current_interval = rules_given_field[0];
	for (size_t i = 1; i < num_rules; i++) {
		if (rules_given_field[i] == current_interval) {
			count++;
		} else {
			out.emplace_back(current_interval, count + 1);
			current_interval = rules_given_field[i];
			count = 1;
		}
		if (i == num_rules - 1) {
			out.emplace_back(current_interval, count + 1);
		}
	}

	return out;
}
std::vector<WeightedInterval> Utilities::CreateUniqueInterval(
		const std::vector<Rule>& rules, int field) {
	// sort partition by field j
	std::multiset<Range1dWeighted> sorted_rules;
	int i = 0;
	for (const auto r : rules) {
		sorted_rules.insert(Range1dWeighted(r.range[field], i++));
	}

	std::vector<WeightedInterval> out;
	out.reserve(rules.size());
	while (!sorted_rules.empty()) {
		auto first_element = begin(sorted_rules);
		auto pit = sorted_rules.equal_range(*first_element);
		std::vector<Rule> temp_rules;
		temp_rules.reserve(rules.size());
		for (auto itr = pit.first; itr != pit.second; ++itr) {
			temp_rules.push_back(rules[itr->id]);
		}
		out.push_back(WeightedInterval(temp_rules, field));
		sorted_rules.erase(pit.first, pit.second);
	}

	return out;
}

void Utilities::_MWISIntervals_endpoints(std::vector<EndPoint> & endpoints) {
	std::sort(begin(endpoints), end(endpoints));
	//add perturbation to make points unique
	size_t index_start_no_change = 0, index_end_no_change = 0;
	for (size_t i = 0; i < endpoints.size() - 1; i++) {
		if (endpoints[i].val != endpoints[i + 1].val) {
			index_end_no_change = i;
			if (index_start_no_change != index_end_no_change) {
				double add_right_endpoint = EPS;
				double subtract_left_endpoint = -EPS;
				for (size_t k = index_start_no_change; k <= index_end_no_change;
						k++) {
					if (endpoints[k].isRightEnd) {
						endpoints[k].val += add_right_endpoint;
						add_right_endpoint += EPS;
					} else {
						endpoints[k].val += subtract_left_endpoint;
						subtract_left_endpoint -= EPS;
					}
				}
			}
			index_start_no_change = i + 1;
		}
	}
	if (index_start_no_change != endpoints.size() - 1) {
		double add_right_endpoint = EPS;
		double subtract_left_endpoint = -EPS;
		for (size_t k = index_start_no_change; k <= endpoints.size() - 1; k++) {
			if (endpoints[k].isRightEnd) {
				endpoints[k].val += add_right_endpoint;
				add_right_endpoint += EPS;
			} else {
				endpoints[k].val += subtract_left_endpoint;
				subtract_left_endpoint -= EPS;
			}
		}
	}
	std::sort(begin(endpoints), end(endpoints));
}

std::pair<std::vector<int>, int> Utilities::MWISIntervals(
		const std::vector<WeightedInterval>&I) {
	const int LOW = 0;
	const int HIGH = 1;
	//need to sort id by endpoints
	int temp_max = 0;
	int max_MIS = 0;
	std::vector<int> S_max;
	S_max.reserve(I.size());
	int last_interval = 0;
	std::vector<int> chi(I.size(), 0);

	std::vector<std::pair<WeightedInterval, int>> sorted_I;
	sorted_I.reserve(I.size());
	for (int i = 0; i < (int) I.size(); i++) {
		sorted_I.push_back(std::make_pair(I[i], i));
	}

	sort(begin(sorted_I), end(sorted_I),
			[](const std::pair<WeightedInterval, int>& lhs, const std::pair<WeightedInterval, int>& rhs) -> bool {
		return lhs.first.high < rhs.first.high;
	});

	std::vector<EndPoint> endpoints;
	for (size_t i = 0; i < sorted_I.size(); i++) {
		endpoints.push_back(EndPoint(sorted_I[i].first.low, LOW, i));
		endpoints.push_back(EndPoint(sorted_I[i].first.high, HIGH, i));
	}

	_MWISIntervals_endpoints(endpoints);

	for (const auto& e : endpoints) {
		if (!e.isRightEnd) {
			chi[e.id] = temp_max + sorted_I[e.id].first.GetWeight();
		} else {
			if (chi[e.id] > temp_max) {
				temp_max = chi[e.id];
				last_interval = e.id;
			}
		}
	}

	S_max.push_back(sorted_I[last_interval].second);
	max_MIS = temp_max;
	temp_max = temp_max - sorted_I[last_interval].first.GetWeight();
	for (int j = last_interval - 1; j >= 0; j--) {
		auto tmp = sorted_I[j];
		if (chi[j] == temp_max
				&& tmp.first.high < sorted_I[last_interval].first.low) {
			S_max.push_back(tmp.second);
			temp_max = temp_max - tmp.first.GetWeight();
			last_interval = j;
		}
	}

	return std::make_pair(S_max, max_MIS);
}
