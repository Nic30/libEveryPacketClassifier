#pragma once

#include <vector>
#include <unordered_map>
#include <limits>
#include <boost/range/adaptor/reversed.hpp>


#include "../ElementaryClasses.h"
#include "partitionSortTree.h"
#include "bitUtils.h"
#include "threadPool.h"

/**
 * The PartitionSort classifier optimized for hardware with
 * parallel implementation of the update methods
 *
 * @ivar dim_t type of the dimension used by classifier
 * @ivar rule_id_t type of the rule id
 * @ivar DIM_CNT number of the dimensions used by this classifier
 * @ivar UPDATE_THR_CNT number of update threads
 **/
template<typename dim_t, typename rule_id_t, size_t DIM_CNT,
		size_t UPDATE_THR_CNT = 0, size_t TREE_CNT = 16, size_t MAX_TREE_SIZE =
				std::numeric_limits<uint16_t>::max(), size_t MAX_BATCH_SIZE =
				32>
class FaRFilter {
	// type of the tree used for classification
	using Tree = PartitionSortTree<dim_t, rule_id_t, DIM_CNT, MAX_TREE_SIZE>;
	// type of the mask for the rule, used in rule compatibility heuristics
	using mask_t = typename Tree::value_type;
	struct mask_hasher {
		typedef mask_t argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& v) const noexcept {
			result_type h = 0;
			for (auto i : v) {
				boost::hash_combine(h, std::hash<dim_t> { }(i));
			}
			return h;
		}
	};
	struct mask_eq {
		typedef mask_t argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& a,
				argument_type const& b) const noexcept {
			for (size_t i = 0; i < DIM_CNT; i++) {
				if (a[i] != b[i])
					return false;
			}
			return true;
		}
	};
	static const constexpr size_t MAX_CMD_PER_TASK =
			UPDATE_THR_CNT ? MAX_BATCH_SIZE / UPDATE_THR_CNT : 1;
	static_assert(UPDATE_THR_CNT == 0 or (MAX_CMD_PER_TASK * UPDATE_THR_CNT >= MAX_BATCH_SIZE));

	// multiple decision trees for classification
	std::array<Tree *, TREE_CNT> trees;

	// dictionary to find tree for rule
	std::unordered_map<Rule, Tree *> rule_to_tree;

	// [TODO] use multiset
	// dictionary to find the probably most suitable tree for the rule
	using mask_to_tree_map_t = std::unordered_map<mask_t, Tree *, mask_hasher, mask_eq>;
	mask_to_tree_map_t mask_to_tree;

	ThreadPool thread_pool;

	class CompatibilityCheckTask {
	public:
		const mask_to_tree_map_t * mask_to_tree;
		const Rule * rules;
		Tree * trees[MAX_CMD_PER_TASK];
		size_t tree_indexes[MAX_CMD_PER_TASK];
		mask_t masks[MAX_CMD_PER_TASK];
		bool colliding[MAX_CMD_PER_TASK];
		size_t rules_cnt;
		__attribute__((aligned(64)))  std::atomic<bool> done;

		CompatibilityCheckTask() :
				mask_to_tree(nullptr), rules(nullptr), rules_cnt(0), done(false) {
		}

		void try_lookup_local() {
			for (size_t i = 0; i < rules_cnt; i++) {
				auto & mask = masks[i];
				auto & rule = rules[i];
				auto & tree = trees[i];

				mask = get_mask(rule);
				auto prioritized_tree = mask_to_tree->find(mask);
				tree = prioritized_tree != mask_to_tree->end() ?
						prioritized_tree->second : nullptr;
				colliding[i] = tree ? tree->check_collision(rule) : false;
			}
			done = true;
		}

		constexpr void apply(std::array<bool, TREE_CNT> & insert_compatible) {
			for (size_t i = 0; i < rules_cnt; i++)
				insert_compatible[tree_indexes[i]] = not colliding[i];
		}

		constexpr bool try_lookup(ThreadPool & thread_pool,
				const mask_to_tree_map_t & _mask_to_tree) {
			mask_to_tree = &_mask_to_tree;
			boost::function<void()> job(
					boost::bind(&CompatibilityCheckTask::try_lookup_local,
							this));
			return thread_pool.run_job(job);
		}
	};

	/*
	 * Extract mask from the rule
	 **/
	static mask_t get_mask(const Rule & rule) {
		mask_t mask;
		for (size_t i = 0; i < DIM_CNT; i++) {
			auto dim_ramge = rule.range[i];
			dim_t m = std::numeric_limits<dim_t>::max();
			// [TODO] highly sub optimal use std::mismatch
			dim_t last_m = m;
			for (size_t i2 = 0; i2 < sizeof(dim_t) * 8; i2++) {
				m <<= 1;
				if ((dim_ramge.low & m) != (dim_ramge.high & m))
					break;
				last_m = m;
			}
			mask[i] = last_m;
		}
		return mask;
	}

	/*
	 * Get the smallest most compatible tree
	 * or the prioritized tree if is compatible
	 *
	 * @param rule rule which we are searching the best tree for
	 * @param mask mask of the rule
	 * @param best_tree best_tree so far
	 *
	 * @return the best suitable tree for the rule
	 ***/
	constexpr Tree* _insert_get_best_tree(const Rule & rule,
			const mask_t & mask, Tree * best_tree = nullptr) {
		Tree * checked_tree = best_tree;
		if (not best_tree or best_tree->check_collision(rule)) {
			Tree * smallest_tree = nullptr;
			for (auto t : boost::adaptors::reverse(trees)) {
				if (t == checked_tree) {
					continue; // it was already checked
				}
				if (not t->check_collision(rule)) {
					best_tree = t;
					break;
				}
				if (not smallest_tree or smallest_tree->size() < t->size()) {
					smallest_tree = t;
				}
			}
			if (not best_tree)
				best_tree = smallest_tree;
		}
		return best_tree;
	}

	/*
	 * Get the smallest most compatible tree
	 * or the prioritized tree if is compatible
	 *
	 * @param insert_compatible flags for the trees if true it means that the tree is compatible
	 * @param prioritized_tree best_tree so far
	 *
	 * @return the best suitable tree for the rule
	 ***/
	constexpr Tree* _insert_get_best_tree(
			const std::array<bool, TREE_CNT> & insert_compatible,
			Tree * prioritized_tree) {
		Tree * smallest_compatible = nullptr;
		Tree * smallest = nullptr;
		for (size_t i = 0; i < TREE_CNT; i++) {
			auto t = trees[i];
			if (insert_compatible[i]) {
				if (t == prioritized_tree) {
					return prioritized_tree;
				}
				if (not smallest_compatible
						or smallest_compatible->size() > t->size())
					smallest_compatible = t;
			} else if (not smallest or smallest->size() > t->size()) {
				smallest = t;
			}
		}
		if (smallest_compatible)
			return smallest_compatible;
		return smallest;
	}

	// register the tree for specified mask
	void _insert_after(const Rule & rule, const mask_t & mask, Tree * tree) {
		mask_to_tree[mask] = tree;
		rule_to_tree[rule] = tree;
	}

	/*
	 * Split
	 *
	 * @return number of tasks generated
	 * */
	constexpr size_t distribute_rule_to_tasks(const Rule * rule, int rules_cnt,
			CompatibilityCheckTask task[MAX_BATCH_SIZE]) {
		if (rules_cnt <= 0)
			return 0;
		int items_per_task = rules_cnt / UPDATE_THR_CNT;
		int items_dept = rules_cnt - (items_per_task * UPDATE_THR_CNT);
		if (items_dept) {
			// to not miss anything and to make distribution more uniform
			// it there is few of items
			items_per_task++;
		}
		// [TODO] multiply the items_per_task to lower number of task if there is too few of items
		int task_offset = 0;
		int task_i = 0;
		while (task_offset < rules_cnt) {
			CompatibilityCheckTask & t = task[task_i];
			t.rules = &rule[task_offset];
			if (task_offset + items_per_task > rules_cnt)
				t.rules_cnt = rules_cnt - task_offset;
			else
				t.rules_cnt = items_per_task;

			task_offset += items_per_task;
			task_i++;
		}
		return task_i;
	}

public:
	void insert(const std::vector<Rule> & rules) {
		for (size_t consumed = 0; consumed < rules.size();) {
			size_t s = rules.size() - consumed;
			if (s > MAX_BATCH_SIZE)
				s = MAX_BATCH_SIZE;
			insert(&rules[consumed], s);
			consumed += s;
		}

	}
	// [TODO] insert vs remove is not thread safe due rule_to_tree
	//        rule can potentially stay in classifier
	// (delete, insert has to be performed from same thread)
	void insert(const Rule * rules, size_t rules_cnt) {
		assert(rules_cnt <= MAX_BATCH_SIZE);
		if (UPDATE_THR_CNT == 0) {
			// sequential
			for (size_t i = 0; i < rules_cnt; i++) {
				auto & rule = rules[i];
				auto mask = get_mask(rule);
				auto _best_tree = mask_to_tree.find(mask);
				Tree * best_tree =
						_best_tree != mask_to_tree.end() ?
								_best_tree->second : nullptr;
				best_tree = _insert_get_best_tree(rule, mask, best_tree);
				best_tree->insert(rule);
				_insert_after(rule, mask, best_tree);
			}
		} else {
			// parallel
			CompatibilityCheckTask task[MAX_BATCH_SIZE];
			size_t task_cnt = distribute_rule_to_tasks(rules, rules_cnt, task);
			for (size_t i = 0; i < task_cnt; i++) {
				// O(d*w + 1 + d log n)
				task[i].try_lookup(thread_pool, mask_to_tree);
			}

			// now we have found the best matching tree by heuristic if exists
			for (size_t i = 0; i < task_cnt; i++) {
				CompatibilityCheckTask & t = task[i];
				while (not t.done)
					;
				// it the the tree for the rule was not found it is
				// insert in to smallest compatible tree
				for (size_t i = 0; i < t.rules_cnt; i++) {
					auto & r = t.rules[i];
					Tree * best_tree = nullptr;
					if (t.trees[i] == nullptr) {
						// tree was not found and we have to search the smallest compatible tree
						// to insert the rule
						for (int i2 = TREE_CNT - 1; i2 >= 0; i2--) {
							Tree * tr = trees[i2];
							if (not tr->check_collision(r) or i2 == 0) {
								best_tree = tr;
								break;
							}
						}
					} else {
						// insert to tree resolved by task
						best_tree = t.trees[i];
					}
					// [TODO] problem is that rules can be separated to groups
					//        for spefic trees and inserted in batches because
					//        of the changes of trees
					// O(d log n)
					best_tree->insert(r);
					_insert_after(r, t.masks[i], best_tree);
				}
			}
		}

	}

	void remove(const Rule & rule) {
		auto tree = rule_to_tree.find(rule);
		if (tree != rule_to_tree.end()) {
			tree->second->remove(rule);
		}
	}

	FaRFilter() :
			thread_pool(UPDATE_THR_CNT) {
		typename Tree::value_type natural_field_order;
		for (size_t i = 0; i < DIM_CNT; i++)
			natural_field_order[i] = i;

		for (size_t i = 0; i < TREE_CNT; i++) {
			trees[i] = new Tree(natural_field_order);
		}
	}

	std::vector<typename Tree::MemoryReport> get_memory_report() const {
		using Rep = typename Tree::MemoryReport;
		std::vector<Rep> res;
		for (auto t : trees) {
			if (t->size() > 0) {
				res.push_back(t->get_memory_report());
			}
		}
		return res;
	}
	;

	~FaRFilter() {
		for (auto t : trees)
			delete t;
	}
// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const FaRFilter & fil) {
		str << "graph FaRFilter {" << std::endl;
		size_t node_i = 0;
		for (auto t : fil.trees) {
			node_i = Tree::print_to_stream(str, *t, node_i, false);
		}
		str << "}";
		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

};
