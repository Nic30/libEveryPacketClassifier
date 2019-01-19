#pragma once

#include "compressedDecisionTree.h"
#include "RBTree.h"

template<typename rule_t>
class CompressedDecisionTreeCompiler {
	using ComprTree = CompressedDecisionTree<uint8_t, rule_t::size()>;

	ComprTree compile(const LLRBTree<rule_t> & llrb_tree) {
		// [TODO] check size expression
		ComprTree c_tree(llrb_tree.size() * rule_t::size());
		// How to check ranges?
		// problem is that in the tree the end is not the next or parent node the end is
		// in the node itself and it can contain the long value over multiple dimmensions
        //


	}

};
