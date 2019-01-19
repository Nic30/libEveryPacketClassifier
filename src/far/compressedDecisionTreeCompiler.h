#pragma once

#include "compressedDecisionTree.h"
#include "llrbTree.h"

template<typename rule_t>
class CompressedDecisionTreeCompiler {
public:
	using ComprTree = CompressedDecisionTree<rule_t::SIZE>;
	using FieldOrder = std::array<uint8_t, rule_t::SIZE>;

	ComprTree * compile(const LLRBTree<rule_t> & llrb_tree, const FieldOrder & field_order) {
		// [TODO] check size expression
		auto c_tree = new ComprTree(llrb_tree.size(), field_order);
		throw std::runtime_error("not implemented");
		return c_tree;
	}

	void compress_node(typename ComprTree::MaxNode * root,
			typename ComprTree::value_type low,
			typename ComprTree::value_type high) {

	}

};
