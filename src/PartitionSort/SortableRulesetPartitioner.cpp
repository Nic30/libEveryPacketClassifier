#include "SortableRulesetPartitioner.h"
#include "../Utilities/IntervalUtilities.h"
#include <assert.h>
using namespace std;

pair<vector<SortableRulesetPartitioner::part>, bool> SortableRulesetPartitioner::IsThisPartitionSortable(
		const part& apartition, int current_field) {
	vector<WeightedInterval> wi = Utilities::CreateUniqueInterval(apartition,
			current_field);
	multiset<unsigned int> low, hi;
	for (auto & i : wi) {
		low.insert(i.GetLow());
		hi.insert(i.GetHigh());
	}
	vector<part> new_partitions;
	for (auto& w : wi) {
		part temp_partition_rule;
		for (auto& r : w.GetRules()) {
			temp_partition_rule.push_back(r);
		}
		new_partitions.push_back(temp_partition_rule);
	}
	return make_pair(new_partitions, Utilities::GetMaxOverlap(low, hi) == 1);
}
pair<vector<SortableRulesetPartitioner::part>, bool> SortableRulesetPartitioner::IsEntirePartitionSortable(
		const vector<part>& all_partition, int current_field) {
	vector<part> new_entire_partition;
	for (const auto& p : all_partition) {
		auto pvi = IsThisPartitionSortable(p, current_field);
		bool isSortable = pvi.second;
		if (!isSortable)
			return make_pair(new_entire_partition, false);
		new_entire_partition.insert(end(new_entire_partition), begin(pvi.first),
				end(pvi.first));
	}
	return make_pair(new_entire_partition, true);
}

bool SortableRulesetPartitioner::IsBucketReallySortable(
		const SortableRuleset & b) {
	vector<part> all_partitions;
	all_partitions.push_back(b.GetRule());
	for (int f : b.GetFieldOrdering()) {
		auto p = IsEntirePartitionSortable(all_partitions, f);
		bool isSortable = p.second;
		if (!isSortable)
			return false;
		all_partitions = p.first;
	}
	return true;
}

pair<vector<SortableRulesetPartitioner::part>, int> SortableRulesetPartitioner::MWISonPartition(
		const part& apartition, int current_field) {
	vector<WeightedInterval> wi = Utilities::CreateUniqueInterval(apartition,
			current_field);
	//printf("Field %d\n", current_field);
	auto mwis = Utilities::MWISIntervals(wi);

	vector<part> vp;
	vector<WeightedInterval> temp_wi;
	vector<part> new_partitions;
	for (int i : mwis.first) {
		part temp_partition_rule;
		for (auto& r : wi[i].GetRules()) {
			temp_partition_rule.push_back(r);
		}
		new_partitions.push_back(temp_partition_rule);
	}

	return make_pair(new_partitions, mwis.second);
}

pair<vector<SortableRulesetPartitioner::part>, int> SortableRulesetPartitioner::MWISonEntirePartition(
		const vector<part>& all_partition, int current_field) {
	vector<part> new_entire_partition;
	int sum_weight = 0;
	for (const auto& p : all_partition) {

		auto pvi = MWISonPartition(p, current_field);
		new_entire_partition.insert(end(new_entire_partition), begin(pvi.first),
				end(pvi.first));
		sum_weight += pvi.second;
	}
	return make_pair(new_entire_partition, sum_weight);
}

void SortableRulesetPartitioner::BestFieldAndConfiguration(
		vector<part>& all_partition, vector<int>& current_field,
		int num_fields) {
	vector<SortableRulesetPartitioner::part> best_new_partition;
	int best_so_far_mwis = -1;
	int current_best_field = -1;
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {

			auto mwis = MWISonEntirePartition(all_partition, j);
			if (mwis.second >= best_so_far_mwis) {
				best_so_far_mwis = mwis.second;
				current_best_field = j;
				best_new_partition = mwis.first;
			}
		}
	}
	all_partition = best_new_partition;
	current_field.push_back(current_best_field);
}

pair<bool, vector<int>> SortableRulesetPartitioner::GreedyFieldSelectionTwoIterations(
		const vector<Rule>& rules) {
	if (rules.size() == 0) {
		printf("Warning: GreedyFieldSelection rule size = 0\n ");
	}
	int num_fields = rules[0].dim;

	vector<int> current_field;
	vector<part> all_partitions;

	all_partitions.push_back(rules);
	for (int i = 0; i < 2; i++) {
		BestFieldAndConfiguration(all_partitions, current_field, num_fields);
	}

	vector<Rule> current_rules;
	for (auto& r : all_partitions) {
		current_rules.insert(end(current_rules), begin(r), end(r));
	}
	//fill in the rest of the field in order
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {
			current_field.push_back(j);
		}
	}
	return make_pair(current_rules.size() == rules.size(), current_field);
}
pair<vector<Rule>, vector<int>> SortableRulesetPartitioner::GreedyFieldSelection(
		const vector<Rule>& rules) {
	if (rules.size() == 0) {
		printf("Warning: GreedyFieldSelection rule size = 0\n ");
	}
	int num_fields = rules[0].dim;

	vector<int> current_field;
	vector<part> all_partitions;

	all_partitions.push_back(rules);
	for (int i = 0; i < num_fields; i++) {
		BestFieldAndConfiguration(all_partitions, current_field, num_fields);
	}

	vector<Rule> current_rules;
	for (auto& r : all_partitions) {
		current_rules.insert(end(current_rules), begin(r), end(r));
	}
	//fill in the rest of the field in order
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {
			current_field.push_back(j);
		}
	}
	return make_pair(current_rules, current_field);
}

vector<SortableRuleset> SortableRulesetPartitioner::SortableRulesetPartitioningGFS(
		const vector<Rule>& rules) {
	vector<Rule> current_rules = rules;
	vector<SortableRuleset> all_buckets;
	for (size_t i = 0; i < current_rules.size(); i++)
		current_rules[i].id = i;

	while (!current_rules.empty()) {
		for (size_t i = 0; i < current_rules.size(); i++)
			current_rules[i].tag = i;
		auto rule_and_field_order = GreedyFieldSelection(current_rules);
		for (auto& r : rule_and_field_order.first) {
			current_rules[r.tag].markedDelete = 1;
		}

		if (rule_and_field_order.first.size() == 0) {
			throw runtime_error(
					"Warning rule_and_field_order.first = 0 in SortableRulesetPartitioning\n");
		}

		SortableRuleset sb = SortableRuleset(rule_and_field_order.first,
				rule_and_field_order.second);

		all_buckets.push_back(sb);
		current_rules.erase(
				remove_if(begin(current_rules), end(current_rules),
						[](const Rule& r) {
							return r.markedDelete;
						}), end(current_rules));

	}
	return all_buckets;
}

vector<SortableRuleset> SortableRulesetPartitioner::AdaptiveIncrementalInsertion(
		const vector<Rule>& rules, int threshold) {
	vector<SortableRuleset> all_buckets;

	auto InsertRuleIntoAllBucket =
			[threshold](const Rule& r, vector<SortableRuleset>& b) {
				for (auto& bucket : b) {
					if (bucket.InsertRule(r)) {
						if (bucket.size() < threshold) {
							//reconstruct
							auto temp_rules = bucket.GetRule();
							auto result = SortableRulesetPartitioner::GreedyFieldSelection(temp_rules);
							bucket = SortableRuleset(result.first, result.second);
						}
						return;
					}
				}
				vector<int> field_order0(r.dim);
				if (threshold == 0) {
					iota(field_order0.begin(), field_order0.end(), 0);
					field_order0 = Random::shuffle_vector(field_order0);
				}
				else {
					field_order0 = SortableRulesetPartitioner::GetFieldOrderByRule(r);
				}

				vector<Rule> vr = {r};
				b.emplace_back(vr, field_order0);
			};

	for (const auto& r : rules) {
		InsertRuleIntoAllBucket(r, all_buckets);
	}

	return all_buckets;
}

vector<int> SortableRulesetPartitioner::GetFieldOrderByRule(const Rule& r) {
	vector<unsigned> rank(r.dim, 0);
	// 0 -> point, 1 -> shorter than half range, 2 -> longer than half range
	//assign rank to each field
	for (int i = 0; i < r.dim; i++) {
		int _r = 0;
		auto length = r.range[i].high - r.range[i].low + 1;
		switch (i) {
			case FieldSA:
			case FieldDA:
				if (length == 1)
					_r = 0;
				else if (length > 0 && length < (1u << 31))
					_r = 1;
				else if (length == 0)
					_r = 3;
				else
					_r = 2;
				break;
			case FieldSP:
			case FieldDP:
				if (length == 1)
					_r = 0;
				else if (length < (1 << 15))
					_r = 1;
				else if (length < (1 << 16))
					_r = 2;
				else
					_r = 3;
				break;
			case FieldProto:
				if (length == 1)
					_r = 0;
				else if (length < (1 << 7))
					_r = 1;
				else if (length < 256)
					_r = 2;
				else
					_r = 3;
				break;
			default:
				throw std::runtime_error("Not implemented for this dimmension");
		}
		rank[i] = _r;
	}
	return sort_indexes(rank);
}

int SortableRulesetPartitioner::ComputeMaxIntersection(
		const vector<Rule>& ruleset) {
	if (ruleset.size() == 0)
		cerr << "Warning SortableRulesetPartitioner::ComputeMaxIntersection: Empty ruleset" << std::endl;
	//auto  result = SortableRulesetPartitioner::GreedyFieldSelection(ruleset);
	auto field_order = { 0, 1, 2, 3, 4 };	 //result.second;
	//reverse(begin(field_order), end(field_order));
	return ComputeMaxIntersectionRecursive(ruleset, ruleset[0].dim - 1,
			field_order);
}

int SortableRulesetPartitioner::ComputeMaxIntersectionRecursive(
		const vector<Rule>& rules, int field_depth,
		const vector<int>& field_order) {

	if (rules.size() == 0)
		return 0;
	if (field_depth <= 0) {
		multiset<Point1d> Astart;
		multiset<Point1d> Aend;
		vector<Rule> rules_rr = Utilities::RedundancyRemoval(rules);
		for (auto v : rules_rr) {
			Astart.insert(v.range[field_order[field_depth]].low);
			Aend.insert(v.range[field_order[field_depth]].high);
		}
		int max_olap = Utilities::GetMaxOverlap(Astart, Aend);
		return max_olap;
	}

	vector<pair<Point1d, Point1d>> start_pointx;
	vector<pair<Point1d, Point1d>> end_pointx;

	vector<Point1d> queries;
	for (Point1d i = 0; i < rules.size(); i++) {
		auto r = rules[i].range[field_order[field_depth]];
		start_pointx.push_back({r.low, i});
		end_pointx.push_back({r.high, i});
		queries.push_back(r.low);
		queries.push_back(r.high);
	}

	sort(begin(start_pointx), end(start_pointx));
	sort(begin(end_pointx), end(end_pointx));
	sort(begin(queries), end(queries));

	size_t i_s = 0;
	size_t i_e = 0;
	int max_POM = 0;
	vector<Rule> rule_this_loop;
	vector<int> index_rule_this_loop(rules.size(), -1); //index by priority
	vector<int> inverse_index_rule_this_loop(rules.size(), -1);
	for (unsigned int qx : queries) {
		bool justInserted = 0;
		bool justDeleted = 0;
		//insert as long as start point is equal to qx
		if (start_pointx.size() != i_s) {
			while (qx == start_pointx[i_s].first) {
				index_rule_this_loop[start_pointx[i_s].second] =
						(int) rule_this_loop.size();
				inverse_index_rule_this_loop[(int) rule_this_loop.size()] =
						start_pointx[i_s].second;
				rule_this_loop.push_back(rules[start_pointx[i_s].second]);
				i_s++;
				justInserted = 1;
				if (start_pointx.size() == i_s)
					break;
			}
		}
		if (justInserted) {
			int this_POM = ComputeMaxIntersectionRecursive(rule_this_loop,
					field_depth - 1, field_order);
			max_POM = max(this_POM, max_POM);
		}

		// delete as long as qx  > ie
		if (end_pointx.size() != i_e) {
			while (qx == end_pointx[i_e].first) {
				int priority = end_pointx[i_e].second;
				if (index_rule_this_loop[priority] < 0) {
					throw runtime_error(
							"ERROR in OPTDecompositionRecursive: index_rule_this_loop[priority] < 0 \n");
				}

				int index_to_delete = index_rule_this_loop[priority];
				int index_swap_in_table =
						inverse_index_rule_this_loop[(int) rule_this_loop.size()
								- 1];
				inverse_index_rule_this_loop[index_to_delete] =
						index_swap_in_table;
				index_rule_this_loop[index_swap_in_table] = index_to_delete;
				iter_swap(
						begin(rule_this_loop) + index_rule_this_loop[priority],
						end(rule_this_loop) - 1);
				rule_this_loop.erase(end(rule_this_loop) - 1);

				i_e++;
				justDeleted = 1;
				if (end_pointx.size() == i_e)
					break;
			}
		}
		if (justDeleted) {
			int this_POM = ComputeMaxIntersectionRecursive(rule_this_loop,
					field_depth - 1, field_order);
			max_POM = max(this_POM, max_POM);
		}
	}

	return max_POM;

}
vector<SortableRuleset> SortableRulesetPartitioner::MaximumIndepdenentSetPartitioning(
		const vector<Rule>& rules) {
	vector<Rule> current_rules = rules;
	vector<SortableRuleset> all_buckets;
	//int sum_rank = 0;
	while (!current_rules.empty()) {
		for (int i = 0; i < (int) current_rules.size(); i++)
			current_rules[i].id = i;
		auto out = MaximumIndependentSetAllFields(current_rules);
		vector<Rule> temp_rules = out.first;
		vector<int> field_order = out.second;
		for (auto& r : temp_rules) {
			current_rules[r.id].markedDelete = 1;
		}

		all_buckets.push_back(SortableRuleset(temp_rules, field_order));
		current_rules.erase(
				remove_if(begin(current_rules), end(current_rules),
						[](const Rule& r) {
							return r.markedDelete;}), end(current_rules));
	}
	return all_buckets;
}

pair<vector<Rule>, vector<int>> SortableRulesetPartitioner::MaximumIndependentSetAllFields(
		const vector<Rule>& rules) {
	vector<int> all_fields(rules[0].dim);
	iota(begin(all_fields), end(all_fields), 0);
	vector<Rule> best_so_far_rules(0);
	vector<int> field_order;
	do {
		auto vrules = MaximumIndependentSetGivenField(rules, all_fields);
		if (vrules.size() > best_so_far_rules.size()) {
			best_so_far_rules = vrules;
			field_order = all_fields;
		}
	} while (next_permutation(all_fields.begin(), all_fields.end()));
	return make_pair(best_so_far_rules, field_order);
}

vector<Rule> SortableRulesetPartitioner::MaximumIndependentSetGivenField(
		const vector<Rule>& rules, const vector<int>& fields) {
	return MaximumIndependentSetGivenFieldRecursion(rules, fields, 0, 0, 0).GetRules();
}

WeightedInterval SortableRulesetPartitioner::MaximumIndependentSetGivenFieldRecursion(
		const vector<Rule>& rules, const vector<int>& fields, int depth,
		unsigned a, unsigned b) {
	if (a > b) {
		printf("Error: interval [a,b] where a > b???\n");
		exit(1);
	}
	vector<WeightedInterval> vwi = Utilities::CreateUniqueInterval(rules,
			fields[depth]);
	vector<WeightedInterval> vwi_opt;
	if (depth < rules[0].dim - 1) {
		for (auto& wi : vwi) {
			vwi_opt.push_back(
					MaximumIndependentSetGivenFieldRecursion(wi.GetRules(),
							fields, depth + 1, wi.GetLow(), wi.GetHigh()));
		}
	} else {
		vwi_opt = vwi;
	}
	auto mwis = Utilities::MWISIntervals(vwi_opt);
	vector<Rule> rules_mwis;
	for (int i : mwis.first) {
		for (auto& r : vwi_opt[i].GetRules()) {
			rules_mwis.push_back(r);
		}
	}
	return WeightedInterval(rules_mwis, a, b);
}

//////////
pair<vector<SortableRulesetPartitioner::part>, int> SortableRulesetPartitioner::FastMWISonPartition(
		const part& apartition, int field) {

	if (apartition.size() == 0) {
		return make_pair(vector<part>(), 0);
	}
	vector<Range1dWeighted> rules_given_field;
	rules_given_field.reserve(apartition.size());
	int i = 0;
	for (const auto & r : apartition) {
		rules_given_field.emplace_back(r.range[field].low, r.range[field].high,
				i++);
	}
	sort(begin(rules_given_field), end(rules_given_field),
			[](const Range1dWeighted& lhs, const Range1dWeighted& rhs) {
				if (lhs.high < rhs.high) {
					return lhs.high < rhs.high;
				}
				return lhs.low < rhs.low;
			});

	vector<LightWeightedInterval> wi = Utilities::FastCreateUniqueInterval(
			rules_given_field);

	auto mwis = Utilities::FastMWISIntervals(wi);

	vector<part> vp;
	vector<part> new_partitions;
	for (int i : mwis.first) {
		part temp_partition_rule;
		for (auto& ind : wi[i].GetRuleIndices()) {
			temp_partition_rule.push_back(
					apartition[rules_given_field[ind].id]);
		}
		new_partitions.push_back(temp_partition_rule);
	}

	return make_pair(new_partitions, mwis.second);
}

pair<vector<SortableRulesetPartitioner::part>, int> SortableRulesetPartitioner::FastMWISonEntirePartition(
		const vector<part>& all_partition, int current_field) {
	vector<part> new_entire_partition;
	int sum_weight = 0;
	for (const auto& p : all_partition) {

		auto pvi = FastMWISonPartition(p, current_field);
		new_entire_partition.insert(end(new_entire_partition), begin(pvi.first),
				end(pvi.first));
		sum_weight += pvi.second;
	}
	return make_pair(new_entire_partition, sum_weight);
}

void SortableRulesetPartitioner::FastBestFieldAndConfiguration(
		vector<part>& all_partition, vector<int>& current_field,
		int num_fields) {

	vector<SortableRulesetPartitioner::part> best_new_partition;
	int best_so_far_mwis = -1;
	int current_best_field = -1;
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {

			auto mwis = FastMWISonEntirePartition(all_partition, j);
			if (mwis.second >= best_so_far_mwis) {
				best_so_far_mwis = mwis.second;
				current_best_field = j;
				best_new_partition = mwis.first;
			}
		}
	}
	all_partition = best_new_partition;
	current_field.push_back(current_best_field);
}

pair<bool, vector<int>> SortableRulesetPartitioner::FastGreedyFieldSelectionTwoIterations(
		const vector<Rule>& rules) {
	if (rules.size() == 0) {
		printf("Warning: GreedyFieldSelection rule size = 0\n ");
	}
	int num_fields = rules[0].dim;

	vector<int> current_field;
	vector<part> all_partitions;

	all_partitions.push_back(rules);
	for (int i = 0; i < 2; i++) {
		FastBestFieldAndConfiguration(all_partitions, current_field,
				num_fields);
	}

	vector<Rule> current_rules;
	for (auto& r : all_partitions) {
		current_rules.insert(end(current_rules), begin(r), end(r));
	}
	//fill in the rest of the field in order
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {
			current_field.push_back(j);
		}
	}
	return make_pair(current_rules.size() == rules.size(), current_field);
}
pair<vector<Rule>, vector<int>> SortableRulesetPartitioner::FastGreedyFieldSelection(
		const vector<Rule>& rules) {
	if (rules.size() == 0) {
		printf("Warning: GreedyFieldSelection rule size = 0\n ");
	}
	int num_fields = rules[0].dim;

	vector<int> current_field;
	vector<part> all_partitions;

	all_partitions.push_back(rules);
	for (int i = 0; i < num_fields; i++) {
		FastBestFieldAndConfiguration(all_partitions, current_field,
				num_fields);
	}

	vector<Rule> current_rules;
	for (auto& r : all_partitions) {
		current_rules.insert(end(current_rules), begin(r), end(r));
	}
	//fill in the rest of the field in order
	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {
			current_field.push_back(j);
		}
	}
	return make_pair(current_rules, current_field);
}

pair<bool, vector<int>> SortableRulesetPartitioner::FastGreedyFieldSelectionForAdaptive(
		const vector<Rule>& rules) {
	if (rules.size() == 0) {
		printf("Warning: GreedyFieldSelection rule size = 0\n ");
	}
	int num_fields = rules[0].dim;

	vector<int> current_field;
	vector<part> all_partitions;

	all_partitions.push_back(rules);
	for (int i = 0; i < num_fields; i++) {
		FastBestFieldAndConfiguration(all_partitions, current_field,
				num_fields);
	}

	for (int j = 0; j < num_fields; j++) {
		if (find(begin(current_field), end(current_field), j)
				== end(current_field)) {
			current_field.push_back(j);
		}
	}

	return make_pair(all_partitions[0].size() == rules.size(), current_field);
}
