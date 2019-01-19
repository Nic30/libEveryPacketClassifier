#pragma once

#include "compressedDecisionTree.h"
#include "RBTree.h"

template<typename rule_t>
class CompressedDecisionTreeCompiler {
	using ComprTree = CompressedDecisionTree<rule_t::size()>;

	ComprTree compile(const LLRBTree<rule_t> & llrb_tree) {
		// [TODO] check size expression
		ComprTree c_tree(llrb_tree.size() * rule_t::size());

	}

};
