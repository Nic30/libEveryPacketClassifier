#pragma once

#include "OptimizedMITree.h"
#include "../Simulation.h"
#include "SortableRulesetPartitioner.h"

/**
 * PartitionSort algorithm for generating of packet classifier.
 *
 * This algorithm separates rules into sortable sets and build decision trees from them.
 *
 *
 */
class PartitionSort: public PacketClassifier {

public:
	void _ConstructClassifier(const std::vector<Rule>& rules);
	/**
	 * Walk sub-tress and use them for classification.
	 */
	int ClassifyAPacket(const Packet& packet);

	/**
	 * Get a sub-tree where rule is stored, delete it from tree,
	 * sort/delete trees if required and remove rule from rules.
	 */
	void DeleteRule(size_t index);

	/**
	 * Try insert rule in to an existing tree or create new tree if it is not possible.
	 */
	void InsertRule(const Rule& one_rule);

	virtual Memory MemSizeBytes() const override;
	virtual int MemoryAccess() const override;
	virtual size_t NumTables() const override;
	virtual size_t RulesInTable(size_t index) const override;

	~PartitionSort();

protected:
	// multiple decision trees for classification
	std::vector<OptimizedMITree *> mitrees;
	// vector used as a dictionary to find tree for rule
	std::vector<std::pair<Rule, OptimizedMITree *>> rules;

	void InsertionSortMITrees();
};
