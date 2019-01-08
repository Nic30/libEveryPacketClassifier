#pragma once

#include <list>
#include <array>
#include <vector>
#include <unordered_map>

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
 * */
template<typename rule_t>
class RuleGroupResolver {
public:
	using mask_t = typename rule_t::mask_t;

	std::unordered_map<mask_t, std::list<rule_t>, hash_mask<mask_t>,
			hash_mask_eq<mask_t>> groups;
	size_t rule_cnt;

	RuleGroupResolver() :
			rule_cnt(0) {
	}

	// [TODO] exact values seems to be faster, but it is required to check it with more data
	void add_rules(std::vector<rule_t> & rules) {
		for (const rule_t & r : rules) {
			auto m = r.getMask();
			auto iter = groups.find(m);
			if (iter == groups.end()) {
				groups[m] = std::list<rule_t>( { r, });
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
				existing->append(bucket.second);
			}
		}
		rule_cnt += other.rule_cnt;
		// clear not required
		other.groups.clear();
		other.rule_cnt = 0;
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const RuleGroupResolver & t) {
		str << "<RuleGroupResolver items:" << t.rule_cnt << std::endl;
		for (auto const & items : t.groups) {
			str << "[";
			std::ios_base::fmtflags flags_backup( str.flags() );
			for (auto const & k : items.first) {
				str << std::hex << "0x" << k << ", ";
			}
			str.flags(flags_backup);
			str << "] = {";

			for (auto const & v : items.second) {
				str << v << ", ";
			}
			str << std::endl;
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
