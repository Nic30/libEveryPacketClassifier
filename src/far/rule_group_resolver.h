#pragma once

#include <list>
#include <array>
#include <vector>
#include <unordered_map>

template<typename rule_t>
class RuleGroupResolver {
public:
	std::unordered_map<typename rule_t::mask_t, std::list<rule_t>> groups;
	size_t rule_cnt;

	RuleGroupResolver() :
			rule_cnt(0) {
	}

	void add_rules(std::vector<rule_t> & rules) {
		for (const rule_t & r : rules) {
			auto m = r.getMask();
			auto iter = groups.find(m);
			if (iter == groups.end()) {
				iter =
						groups.insert(groups.value_type(m, std::list<rule_t>())).second;
			}
			iter.push_back(r);
			rule_cnt++;
		}
	}

	void merge(RuleGroupResolver<rule_t> & other) {
		for (auto & bucket : other.groups.begin()) {
			auto existing = groups.find(bucket.first);

			if (existing == groups.end()) {
				groups[bucket.first] = bucket.second;
			} else {
				existing->append(bucket.second);
			}
		}
		rule_cnt += other.rule_cnt;
		// clear not required
		other.groups.clear();
		other.rule_cnt = 0;
	}

};
