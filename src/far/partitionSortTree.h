#pragma once

#include <boost/intrusive/set.hpp>
#include <cassert>

#include "../ElementaryClasses.h"
#include "rbtree_printer.h"

//https://www.boost.org/doc/libs/1_58_0/doc/html/intrusive/node_algorithms.html#intrusive.node_algorithms.rbtree_algorithms
//https://stackoverflow.com/questions/31557312/how-to-access-the-root-of-rbtree-in-boost

using namespace boost::intrusive;

template<typename dim_t, typename rule_id_t, size_t DIM_CNT>
class PartitionSortTree {
public:
	using value_type = std::array<dim_t, DIM_CNT>;
	using rule_type = std::array<_Range1d<dim_t>, DIM_CNT>;
	static constexpr rule_id_t RULE_NOT_FOUND = 0;
	class Node;
	using Tree = rbtree<Node>;
	class Node: public set_base_hook<> {
	public:
		// pointer on next level tree (for other dimension)
		Tree * next_level_tree;
		// range on dimension use as a key in this tree
		Range1d range;
		// rule value for this node
		// (if there is not a path from this node this is our matching rule)
		rule_id_t rule;
		Node(Range1d range, rule_id_t rule) :
				next_level_tree(nullptr), range(range), rule(rule) {
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
		~Node() {
			delete next_level_tree;
		}

	};
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

	Tree root;
	size_t _M_size;
	value_type field_order;

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

	void insert(const rule_type & rule, rule_id_t id) noexcept {
		auto * tree = &root;
		const size_t prefix_len = get_index_of_last_specified_dimension(rule);
		assert(prefix_len <= DIM_CNT);

		for (size_t i = 0; i < prefix_len; i++) {
			bool is_last_level = i == prefix_len - 1;
			Node tmp_node(rule[i], is_last_level);
			typename Tree::insert_commit_data d;
			std::pair<typename Tree::iterator, bool> res =
					tree->insert_unique_check(tmp_node,
							typename Tree::key_compare(), d);
			Node* n = nullptr;
			if (res.second) {
				n = new Node(rule[i], is_last_level);
				tree->insert_unique_commit(*n, d);
			} else {
				n = &*res.first;
			}
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

	void remove(const rule_type & rule, rule_id_t rule_id) {
		// subtre and node in subtree
		std::pair<Tree*, Node*> path_in_tree[DIM_CNT];
		size_t i0 = 0;
		Tree * r = &root;
		// discover the path in the tree which represent the rule
		for (; i0 < DIM_CNT; i0++) {
			auto n = r->find(Node(rule[i0], rule_id));
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
	}

	constexpr const Node * find_in_subtree(const Tree * tree, dim_t value) const
			noexcept {
		auto _n = tree->root();
		if (_n == tree->end()) {
			return nullptr;
		}
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
		return nullptr;
	}

	/*
	 * Iterate tree levels and search the part of the value in tree on specified level
	 */
	constexpr rule_id_t find(const value_type & value) const
			noexcept {
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

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const PartitionSortTree & t) {
		size_t node_i = 0;
		TreePrinter printer;
		// <parent, subtree>
		std::vector<std::pair<const Node *, const Tree *>> trees_on_current_level;
		std::vector<std::pair<const Node *, const Tree *>> trees_on_next_level;
		trees_on_current_level.push_back( { nullptr, &t.root });

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

		str << "}";

		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

};
