#pragma once

#include <vector>
#include <unordered_map>
#include <limits>

#include "../ElementaryClasses.h"
#include "partitionSortTree.h"
#include "bitUtils.h"
#include "concurrent_queue.h"
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
		size_t UPDATE_THR_CNT = 0, size_t TREE_CNT = 8, size_t MAX_TREE_SIZE =
				std::numeric_limits<uint16_t>::max()>
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

	// multiple decision trees for classification
	std::array<Tree *, TREE_CNT> trees;

	// dictionary to find tree for rule
	std::unordered_map<Rule, Tree *> rule_to_tree;

	// dictionary to find the probably most suitable tree for the rule
	std::unordered_map<mask_t, Tree *, mask_hasher, mask_eq> mask_to_tree;

	ThreadPool thread_pool;

	class CompatibilityCheckRequest {
	public:
		const Rule * rule;
		const Tree * tree;
		size_t tree_index;
		bool colliding;
		bool started;
		__attribute__((aligned(64))) std::atomic<bool> done;

		CompatibilityCheckRequest() :
				rule(nullptr), tree(nullptr), tree_index(0), colliding(true), started(
						false), done(false) {
		}
		CompatibilityCheckRequest(const Rule & rule, const Tree * tree) :
				rule(&rule), tree(tree), tree_index(0), colliding(true), started(
						false), done(false) {
		}

		void _start() {
			started = true;
			colliding = tree->check_collision(*rule);
			done = true;
		}

		void apply(std::array<bool, TREE_CNT> & insert_compatible) {
			insert_compatible[tree_index] = not colliding;
		}

		bool start(ThreadPool & thread_pool) {
			boost::function<void()> job(
					boost::bind(&CompatibilityCheckRequest::_start, this));
			return thread_pool.run_job(job);
		}
	};

	/*
	 * Extract mask from the rule
	 **/
	mask_t get_mask(const Rule & rule) {
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
	Tree* _insert_get_best_tree(const Rule & rule, const mask_t & mask,
			Tree * best_tree = nullptr) {
		Tree * checked_tree = best_tree;
		if (not best_tree or best_tree->check_collision(rule)) {
			Tree * smallest_tree = nullptr;
			for (auto t : trees) {
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
	Tree* _insert_get_best_tree(
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

	/*
	 * Get the smallest most compatible tree
	 * or the prioritized tree if is compatible
	 * and optionally ask worker thread for some checks
	 *
	 **/
	Tree* _insert_get_best_tree_distributed(const Rule & rule,
			const mask_t & mask, Tree * prioritized_tree) {
		constexpr size_t TREE_SIZE_TRESHOLD = 20;
		std::array<bool, TREE_CNT> insert_compatible;

		CompatibilityCheckRequest prioritized_tree_check(rule,
				prioritized_tree);
		if (prioritized_tree) {
			if (prioritized_tree->size() < TREE_SIZE_TRESHOLD) {
				if (not prioritized_tree->check_collision(rule))
					return prioritized_tree;
			}

			// ask the worker as the tree is too large
			// up to O(d * log n) (n-number of nodes in tree) on worker thread
			while (not prioritized_tree_check.start(thread_pool))
				;
		}

		// check if there is a small compatible tree
		// the check is performed before as potential communication
		// significantly degrades performance if the operation is too simple
		for (auto t : trees) {
			if (t != prioritized_tree and t->size() < TREE_SIZE_TRESHOLD
					and not t->check_collision(rule)) {
				return t;
			}
		}

		std::array<CompatibilityCheckRequest, TREE_CNT> checks;
		std::fill(insert_compatible.begin(), insert_compatible.end(), false);
		// check if the worker has finished checking the prioritized tree
		if (prioritized_tree_check.started) {
			assert(prioritized_tree != nullptr);
			while (not prioritized_tree_check.done)
				;
			if (not prioritized_tree_check.colliding)
				return prioritized_tree;
		}

		// spot tree compatibility query because all trees are large or not compatible
		size_t req_cnt = 0;
		size_t i = 0;
		for (auto t : trees) {
			// skip the prioritized_tree  as we already checked it
			if (t != prioritized_tree and t->size() >= TREE_SIZE_TRESHOLD) {
				// up to O(d * log n) (n-number of nodes in tree) on worker thread
				CompatibilityCheckRequest & check = checks[req_cnt];
				check.rule = &rule;
				check.tree = t;
				check.tree_index = i;
				while (not check.start(thread_pool))
					;
				req_cnt++;
			}
			i++;
		}

		// collect the tree compatibility results
		for (size_t i = 0; i < req_cnt; i++) {
			CompatibilityCheckRequest & check = checks[i];
			while (not check.done)
				;
			check.apply(insert_compatible);
		}
		if (prioritized_tree_check.started) {
			while (not prioritized_tree_check.done)
				;
			prioritized_tree_check.apply(insert_compatible);
		}
		// resolve the tree where rule should be stored
		Tree * best_tree = _insert_get_best_tree(insert_compatible,
				prioritized_tree);

		assert(best_tree != nullptr);
		//while (not thread_pool.empty()) {
		//	std::cout << "waiting on children" << std::endl;
		//}

		return best_tree;
	}

	// register the tree for specified mask
	void _insert_after(const Rule & rule, const mask_t & mask, Tree * tree) {
		mask_to_tree[mask] = tree;
		rule_to_tree[rule] = tree;
	}

public:
	// [TODO] insert vs remove is not thread safe due rule_to_tree
	//        rule can potentially stay in classifier
	void insert(const Rule & rule) {
		Tree * best_tree = nullptr;
		mask_t mask = get_mask(rule);
		auto prioritized_tree = mask_to_tree.find(mask);
		if (prioritized_tree != mask_to_tree.end())
			best_tree = prioritized_tree->second;

		if (UPDATE_THR_CNT > 0) {
			// O(d log n) on worker thread max(O(d log n), O(d w)) on main thread
			best_tree = _insert_get_best_tree_distributed(rule, mask,
					best_tree);
			// O(d^2 log n) on worker thread
			// confirmation is not required as the potential delete would wait on spinlocks in tree
			// and the commands in queue are ordered
			//while (not update_queue_req.push( { 0, &rule, UpdateRequest::INSERT,
			//		best_tree, }))
			//	;
		} else {
			best_tree = _insert_get_best_tree(rule, mask, best_tree);
		}
		best_tree->insert(rule);
		_insert_after(rule, mask, best_tree);
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
