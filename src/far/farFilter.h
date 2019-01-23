#pragma once

#include <vector>
#include <unordered_map>
#include "../ElementaryClasses.h"
#include "partitionSortTree.h"

template<typename dim_t, typename rule_id_t, size_t DIM_CNT>
class FaRFilter {
	using Tree = PartitionSortTree<dim_t, rule_id_t, DIM_CNT>;
	// multiple decision trees for classification
	std::vector<Tree *> trees;
	// dictionary to find tree for rule
	std::unordered_map<Rule, Tree *> rules;
};
