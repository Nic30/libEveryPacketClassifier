#pragma once

#include "compressedDecisionTree.h"
#include "llrbTree.h"

template<typename rule_t>
class CompressedDecisionTreeCompiler {
public:
	using ComprTree = CompressedDecisionTree<rule_t::SIZE>;
	using FieldOrder = std::array<uint8_t, rule_t::SIZE>;

	/*
	 * Strip off the redundant check and data, convert llrb tree to more memory friendly format
	 *
	 * :note: For compilation the field order is not required,
	 *        the field order is specified by the order in tree.
	 *        The field_order argument is present to ensure object consistency.
	 *
	 * */
	ComprTree * compile(const LLRBTree<rule_t> & llrb_tree, const FieldOrder & field_order) {
		// [TODO] check size expression
		auto c_tree = new ComprTree(llrb_tree.size(), field_order);
		throw std::runtime_error("not implemented");
		return c_tree;
	}

	void compress_node(typename ComprTree::MaxNode * root,
			const typename ComprTree::value_type & low,
			const typename ComprTree::value_type & high) {

	}

};
