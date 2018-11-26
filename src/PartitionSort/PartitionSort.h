#pragma once

#include "OptimizedMITree.h"
#include "../Simulation.h"
#include "SortableRulesetPartitioner.h"

class PartitionSort: public PacketClassifier {

public:
	void ConstructClassifier(const std::vector<Rule>& rules);
	int ClassifyAPacket(const Packet& packet);
	void DeleteRule(size_t index);
	void InsertRule(const Rule& one_rule);

	Memory MemSizeBytes() const;
	int MemoryAccess() const;
	size_t NumTables() const;
	size_t RulesInTable(size_t index) const;

	~PartitionSort();
protected:

	// multiple decision trees for classification
	std::vector<OptimizedMITree *> mitrees;
	// vector used as a dictionary to find tree for rule
	std::vector<std::pair<Rule, OptimizedMITree *>> rules;

	void InsertionSortMITrees();
};
