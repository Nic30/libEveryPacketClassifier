#pragma once

#include <boost/intrusive/set.hpp>
#include <cassert>

#include "../ElementaryClasses.h"
#include "rbtree_printer.h"
#include "mempool.h"
#include <atomic>

using namespace boost::intrusive;

/**
 * Optimized Multidimensional Interval tree from the PartitionSort algorithm
 * This tree is composed of DIM_CNT levels where each level is composed
 * from multiple Red-Black trees, levels are linked together in to this tree
 * For each level there is specified dimension and all nodes on this level
 * of this tree are contain ranges only in this dimension
 * The dimension for each level is specified in field_order
 *
 * @tparam dim_t type of value in dimension
 * @tparam rule_id_t type of the rule id
 * @tparam DIM_CNT number of dimension for the classifier
 *
 **/
template<typename dim_t, typename rule_id_t, size_t DIM_CNT,
		size_t MAX_TREE_SIZE = std::numeric_limits<uint16_t>::max()>
class PartitionSortTree {
public:
	// type which this classifier classifies
	using value_type = std::array<dim_t, DIM_CNT>;
	// type of rule stored in this classifier
	using rule_type = std::array<_Range1d<dim_t>, DIM_CNT>;
	// constant which means that there is not any matching rule
	static constexpr rule_id_t RULE_NOT_FOUND = 0;
	//
	using priority_t = rule_id_t;

	class Node;
	class Tree: public rbtree<Node> {
		__attribute__((aligned(64))) std::atomic_flag update_lock;

	public:
		Tree(): rbtree<Node>(), update_lock(ATOMIC_FLAG_INIT) {
		}
		void update_lock_acquire() {
			while (update_lock.test_and_set(std::memory_order_acquire))
				// acquire lock
				;// spin
		}
		void update_lock_release() {
			update_lock.clear(std::memory_order_release); // release lock
		}
	};

	// Node for
	class Node final: public set_base_hook<>, public ObjectWithStaticMempool<
			Node, MAX_TREE_SIZE> {
	public:
		// pointer on next level tree (for other dimension)
		Tree * next_level_tree;
		// range on dimension use as a key in this tree
		Range1d range;

		// priority for the rule only if specified
		int priority;
		// rule value for this node
		// (if there is not a path from this node this is our matching rule)
		rule_id_t rule;
		Node(Range1d range, rule_id_t rule, int priority) :
				next_level_tree(nullptr), range(range), priority(priority), rule(
						rule) {
		}
		friend constexpr bool operator<(const Node& a, const Node& b) {
			return a.range < b.range;
		}

		friend constexpr bool operator>(const Node& a, const Node& b) {
			return a.range > b.range;
		}

		friend constexpr bool operator==(const Node& a, const Node& b) {
			return a.range == b.range;
		}
		// used only during initialization of mempool
		~Node() {
			delete next_level_tree;
		}

	};

	// printer which converts this tree to string in dot format
	class TreePrinter: public RBTreePrinter<Node> {
	public:
		size_t dim_id;
		TreePrinter() :
				dim_id(0) {
		}
		virtual bool is_red(const Node & n) override {
			return n.color_ == Node::red_t;
		}
		virtual void print_key(const Node & n, std::ostream & str) override {
			str << "d[" << dim_id << "]: " << n.range;
		}
		virtual const Node * left(const Node & n) override {
			return reinterpret_cast<const Node *>(n.left_);
		}
		virtual const Node * right(const Node & n) override {
			return reinterpret_cast<const Node *>(n.right_);
		}

	};

	class MemoryReport {
	public:
		class LevelInfo {
		public:
			size_t tree_cnt;
			size_t max_tree_size;
			size_t node_cnt;
			LevelInfo() :
					tree_cnt(0), max_tree_size(0), node_cnt(0) {
			}
		};
		std::array<LevelInfo, DIM_CNT> info;

		// serialize graph to string in dot format
		friend std::ostream & operator<<(std::ostream & str,
				const MemoryReport & self) {
			str << "<MemoryReport " << std::endl;
			for (size_t i = 0; i < DIM_CNT; i++) {
				auto inf = self.info[i];
				str << "   level " << i << ": nodes:" << inf.node_cnt
						<< " tree_cnt" << inf.tree_cnt << " max_tree_size:"
						<< inf.max_tree_size << std::endl;
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

	Tree root;
	size_t _M_size;
	// the specified dimension for each level of this tree
	value_type field_order;
	// [TODO] max_priority for software classifier

	PartitionSortTree(const value_type & field_order) :
			_M_size(0), field_order(field_order) {
	}

	constexpr size_t get_index_of_last_specified_dimension(
			const rule_type & rule) {
		// find last index of the part which is not a wildcard
		for (int i = DIM_CNT - 1; i >= 0; i--) {
			if (not rule[i].isWildcard())
				return i + 1;
		}
		return 0;
	}

	void insert(const Rule & rule) {
		const _Range1d<dim_t> * r = &rule.range[0];
		insert(*reinterpret_cast<const rule_type*>(r), rule.id, rule.priority);
	}

	void insert(const rule_type & rule, rule_id_t id, int priority) noexcept {
		auto * tree = &root;
		const size_t prefix_len = get_index_of_last_specified_dimension(rule);
		assert(prefix_len <= DIM_CNT);

		for (size_t i = 0; i < prefix_len; i++) {
			bool is_last_level = i == prefix_len - 1;
			Node tmp_node(rule[i], is_last_level, priority);
			typename Tree::insert_commit_data d;

			//tree->update_lock_acquire();
			std::pair<typename Tree::iterator, bool> res =
					tree->insert_unique_check(tmp_node,
							typename Tree::key_compare(), d);
			Node* n = nullptr;
			if (res.second) {
				n = new Node(rule[i], is_last_level, priority);
				tree->insert_unique_commit(*n, d);
			} else {
				n = &*res.first;
			}
			//tree->update_lock_release();

			if (not is_last_level and n->next_level_tree == nullptr) {
				tree = n->next_level_tree = new Tree();
			} else {
				n->rule = id;
			}
		}
		_M_size++;
	}
	constexpr size_t size() {
		return _M_size;
	}

	void remove(const Rule & rule) {
		const _Range1d<dim_t> * r = &rule.range[0];
		remove(*reinterpret_cast<const rule_type*>(r), rule.id);
	}

	void remove(const rule_type & rule, rule_id_t rule_id) {
		// subtree and node in subtree
		std::pair<Tree*, Node*> path_in_tree[DIM_CNT];
		size_t i0 = 0;
		Tree * r = &root;
		//root.update_lock_acquire();
		// discover the path in the tree which represent the rule
		for (; i0 < DIM_CNT; i0++) {
			auto n = r->find(Node(rule[i0], rule_id, -1));
			if (n == r->end()) {
				break;
			}
			path_in_tree[i0] = {r, &*n};
			r = n->next_level_tree;
		}
		// reverse iterate and delete also rb subtrees in next level is subtree is empty
		for (int i = i0 - 1; i >= 0; i--) {
			Tree * r;
			Node * n;
			std::tie(r, n) = path_in_tree[i];

			// delete tree in next level if it is empty
			if (n->next_level_tree and n->next_level_tree->empty()) {
				delete n->next_level_tree;
				n->next_level_tree = nullptr;
			}
			// delete this node if nothing depends on it
			if (n->next_level_tree == nullptr) {
				r->erase(*n);
				delete n;
			} else if (n->rule == rule_id)
				n->rule = RULE_NOT_FOUND;
		}
		_M_size--;
		//root.update_lock_release();
	}

	/**
	 * Find the range in rbtree
	 * @return the pair <node, flag which is true if there is a collision>
	 **/
	constexpr std::pair<const Node *, bool> find_in_subtree(const Tree * tree,
			Range1d range) const noexcept {
		auto _n = tree->root();
		if (_n != tree->end()) {
			const Node * n = &*_n;
			while (n) {
				if (range == n->range) {
					return {n, false};
				} else if (range.isIntersect(n->range)) {
					return {n, true};
				} else if (range < n->range) {
					n = reinterpret_cast<const Node *>(n->left_);
				} else if (range > n->range) {
					n = reinterpret_cast<const Node *>(n->right_);
				} else {
					assert(0 && "all relation should have been checked");
				}
			}
		}
		return {nullptr, false};
	}

	/*
	 * Find the value in rbtree
	 **/
	constexpr const Node * find_in_subtree(const Tree * tree, dim_t value) const
			noexcept {
		auto _n = tree->root();
		if (_n != tree->end()) {
			const Node * n = &*_n;
			while (n) {
				if (value < n->range.low) {
					n = reinterpret_cast<const Node *>(n->left_);
				} else if (value > n->range.high) {
					n = reinterpret_cast<const Node *>(n->right_);
				} else {
					return n;
				}
			}
		}
		return nullptr;
	}

	/*
	 * Iterate tree levels and search the part of the value in tree on specified level
	 */
	constexpr rule_id_t find(const value_type & value) const noexcept {
		auto * tree = &root;
		auto res = RULE_NOT_FOUND;
		for (size_t i = 0; i < DIM_CNT; i++) {
			auto dim = field_order[i];
			auto v = value[dim];
			const Node * n = find_in_subtree(tree, v);
			if (n) {
				res = n->rule;
				tree = n->next_level_tree;
				if (tree == nullptr)
					break;
			}
		}
		return res;
	}

	/*
	 * @return true if the rule is colliding with some other in this tree
	 **/
	constexpr bool check_collision(const Rule & rule) const noexcept {
		auto * tree = &root;
		for (size_t i = 0; i < DIM_CNT; i++) {
			auto dim = field_order[i];
			auto v = rule.range[dim];
			const Node * n;
			bool colliding;
			std::tie(n, colliding) = find_in_subtree(tree, v);
			if (colliding)
				return true;
			if (n) {
				tree = n->next_level_tree;
				if (tree == nullptr)
					break;
			}
		}
		return false;
	}

	static size_t print_to_stream(std::ostream & str,
			const PartitionSortTree & t, size_t node_i, bool print_wrap) {
		TreePrinter printer;
		// <parent, subtree>
		std::vector<std::pair<const Node *, const Tree *>> trees_on_current_level;
		std::vector<std::pair<const Node *, const Tree *>> trees_on_next_level;
		trees_on_current_level.push_back( { nullptr, &t.root });

		if (print_wrap)
			str << "graph partition_sort_graph {" << std::endl;
		while (trees_on_current_level.size() > 0) {

			for (auto itm : trees_on_current_level) {
				const Node * parent;
				const Tree * _root;
				std::tie(parent, _root) = itm;
				auto r = _root->root();
				if (r != _root->end()) {
					const Node * rn = &*r;
					node_i = printer.printNode(str, node_i, rn);
				}
				if (parent) {
					const Node * root_of_subtree = &*_root->root();

					printer.printConnection(str, printer.node_to_id[parent],
							printer.node_to_id[root_of_subtree],
							std::string("LEVEL")
									+ std::to_string(printer.dim_id));
				}
				// discover all paths to next level of the tree of RB subtrees
				for (const Node & n : *_root) {
					if (n.next_level_tree) {
						trees_on_next_level.push_back(
								{ &n, n.next_level_tree });
					}
				}
			}

			trees_on_current_level.clear();
			std::swap(trees_on_current_level, trees_on_next_level);
			printer.dim_id++;
		}

		if (print_wrap)
			str << "}";
		return node_i;
	}

	MemoryReport get_memory_report() const {
		std::vector<const Tree *> trees_on_current_level;
		std::vector<const Tree *> trees_on_next_level;
		trees_on_current_level.push_back(&root);
		MemoryReport memRep;
		size_t level = 0;
		while (trees_on_current_level.size() > 0) {
			typename MemoryReport::LevelInfo & m = memRep.info[level];
			for (const Tree * _root : trees_on_current_level) {
				// discover all paths to next level of the tree of RB subtrees
				size_t nodes = 0;
				for (const Node & n : *_root) {
					if (n.next_level_tree) {
						trees_on_next_level.push_back(n.next_level_tree);
					}
					nodes++;
				}
				m.max_tree_size = std::max(m.max_tree_size, nodes);
				m.node_cnt += nodes;
				m.tree_cnt++;
			}
			level++;

			trees_on_current_level.clear();
			std::swap(trees_on_current_level, trees_on_next_level);
		}
		return memRep;
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const PartitionSortTree & t) {
		print_to_stream(str, t, 0, true);

		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

};
