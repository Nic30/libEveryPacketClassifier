#pragma once

#include "compressedDecisionTree.h"
#include "llrbTree.h"

template<typename rule_t, typename rule_id_t = uint16_t,
		typename node_value_t = uint16_t>
class CompressedDecisionTreeCompiler {
public:
	using ComprTree = CompressedDecisionTree<rule_t::SIZE, rule_id_t, node_value_t>;
	using FieldOrder = std::array<uint8_t, rule_t::SIZE>;
	using Tree = LLRBTree<rule_t>;
	using Boundary = typename ComprTree::value_type;
	ComprTree * c_tree;

	CompressedDecisionTreeCompiler() :
			c_tree(nullptr) {
	}

	/*
	 * Strip off the redundant check and data,
	 * convert llrb tree to more memory friendly format
	 *
	 * :note: For compilation the field order is not required,
	 *        the field order is specified by the order in tree.
	 *        The field_order argument is present to ensure object consistency.
	 *
	 **/
	ComprTree * compile(const Tree & tree, const FieldOrder & field_order) {
		c_tree = new ComprTree(tree.size(), field_order);
		Boundary low = { 0 };
		Boundary high = {
				std::numeric_limits<typename Boundary::value_type>::max() };
		if (tree.get_root())
			compress_node(*tree.get_root(), c_tree->root[0], low, high, 0);

		return c_tree;
	}

	/* In-order pass trough tree and store only unique essential
	 * informations from n node to cn node
	 *
	 * :param n: node of decision tree
	 * :param cn: node of compressed decision tree
	 * :param low: the lowest possible value which can get in to this node
	 * :param high: the highest possible value which can get in to this node
	 **/
	void compress_node(const typename Tree::Node_t & n,
			typename ComprTree::MaxNode & cn, const Boundary & low,
			const Boundary & high, uint16_t node_index) {
		// [TODO] check for shared prefix
		cn.dim_cnt = rule_t::SIZE;
		cn.dim_offset = 0;
		cn.rule = n.key.id;

		for (size_t i = 0; i < rule_t::SIZE; i++) {
			const auto & k = n.key[i];
			cn.val[i] = k.low();
			cn.val[cn.dim_cnt + i] = k.high();
		}

		if (n.left) {
			cn.left_vld = true;
			auto _node_index = ComprTree::get_left_index(node_index);
			auto _high = reinterpret_cast<Boundary*>(cn.val);
			compress_node(*n.left, c_tree->root[_node_index], low, *_high,
					_node_index);
		} else {
			cn.left_vld = false;
		}

		if (n.right) {
			cn.right_vld = true;
			auto _node_index = ComprTree::get_right_index(node_index);
			auto _low = reinterpret_cast<Boundary*>(cn.val[rule_t::SIZE - 1]);
			compress_node(*n.right, c_tree->root[_node_index], *_low, high,
					_node_index);
		} else {
			cn.right_vld = false;
		}
	}

	~CompressedDecisionTreeCompiler() {
		delete c_tree;
	}
};
