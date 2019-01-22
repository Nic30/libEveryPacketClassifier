#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include "bitUtils.h"

// templates for hashing of bit mask of rules
template<typename mask_t>
struct hash_mask;

template<typename mask_t>
struct hash_mask {
	typedef mask_t argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& fv) const noexcept {
		result_type h = 0;
		for (const auto & v : fv) {
			boost::hash_combine(h, v);
		}
		return h;
	}
};

template<typename mask_t>
struct hash_mask_eq {
	typedef mask_t argument_type;
	bool operator()(const argument_type& l, const argument_type& r) const {
		for (size_t i = 0; i < l.size(); i++) {
			const auto & a = l[i];
			const auto & b = r[i];
			if (a != b)
				return false;
		}
		return true;
	}
};

/*
 * Groups the rules with similar features to groups
 * :note: feature = prefix masks
 *
 * [TODO] exact values for rule_t (instead reference) seems to be faster,
 *        but it is required to check it with more data
 **/
template<typename rule_t>
class RuleGroupResolver {
public:
	using mask_t = typename rule_t::mask_t;

	std::unordered_map<mask_t, std::vector<rule_t>, hash_mask<mask_t>,
			hash_mask_eq<mask_t>> groups;
	size_t rule_cnt;

	RuleGroupResolver(const RuleGroupResolver & other) = delete;
	RuleGroupResolver() :
			rule_cnt(0) {
	}

	RuleGroupResolver(std::vector<rule_t> & rules) :
			RuleGroupResolver(), rule_cnt(0) {
		add_rules(rules);
	}

	void add_rules(std::vector<rule_t> & rules) {
		for (const rule_t & r : rules) {
			auto m = r.getMask();
			auto iter = groups.find(m);
			if (iter == groups.end()) {
				groups[m] = std::vector<rule_t>( { r, });
				rule_cnt++;
				continue;
			}
			iter->second.push_back(r);
			rule_cnt++;
		}
	}

	void merge(RuleGroupResolver<rule_t> & other) {
		for (auto & bucket : other.groups.begin()) {
			auto existing = groups.find(bucket.first);

			if (existing == groups.end()) {
				groups[bucket.first] = bucket.second;
			} else {
				// [TODO] check inserting smaller vector to larger
				// note: list is much slower and merging happens rarely
				const auto & other = bucket.second;
				existing->insert(existing.end(), other.begin(), other.end());
			}
		}
		rule_cnt += other.rule_cnt;
		// clear not required
		other.groups.clear();
		other.rule_cnt = 0;
	}

	//static void _expand_group(mask_t & mask, mask_t & expand_to,
	//		std::vector<rule_t> & rules) {
	//	assert(not rules.empty());
    //
	//	// count how many bit is expanded per dimmension
	//	std::array<size_t, mask.size()> expand_len;
	//	mask_t expanding_mask;
	//	const auto & r0 = rules.front();
	//	for (size_t i = 0; i < mask.size(); i++) {
	//		auto s = expand_to[i];
	//		expand_len[i] =
	//				s > 0 ? BitUtils<typename mask_t::value_type>::countSetBits(
	//								s) :
	//						0;
	//		expanding_mask[i] = s ^ r0.prefix_mask;
	//	}
	//	// value can remain same as it is extended by 0
	//	// high, prefix_mask has to be extended with 1
    //
	//	// user first rule to get mask length instead of complex computation
	//}

	//void merge_groups(const mask_t & mask0, const mask_t & mask1) {
	//	bool need_expand0 = false, need_expand1 = false;
	//	mask_t exp0, exp1;
	//	for (size_t i = 0; i < mask_t::SIZE; i++) {
	//		auto a = mask0[i], b = mask1[i];
	//		bool _need_expand0 = a < b;
	//		bool _need_expand1 = b < a;
	//		exp0[i] = _need_expand0 ? b ^ a : 0;
	//		exp1[i] = _need_expand1 ? a ^ b : 0;
	//		need_expand0 |= _need_expand0;
	//		need_expand1 |= _need_expand1;
	//	}
    //
	//	if (need_expand0) {
	//		// remove group from groups because it's key has changed
	//		_expand_group(mask0);
	//	}
	//}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const RuleGroupResolver & t) {
		str << "<RuleGroupResolver items:" << t.rule_cnt << std::endl;
		for (auto const & items : t.groups) {
			str << "[";
			std::ios_base::fmtflags flags_backup(str.flags());
			for (auto const & k : items.first) {
				str << std::hex << "0x" << k << ", ";
			}
			str.flags(flags_backup);
			str << "] = {";

			for (auto const & v : items.second) {
				str << v << ", ";
			}
			str << "}" << std::endl;
		}
		str << ">";
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};
