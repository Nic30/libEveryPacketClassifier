#pragma once

#include "red_black_tree.h"
#include "SortableRulesetPartitioner.h"
#include "../Simulation.h"

/*
 * Multidimensional red-black tree
 * */
class OptimizedMITree: public ClassifierTable {
public:
	OptimizedMITree(const SortableRuleset& rules);
	OptimizedMITree(const std::vector<int>& fieldOrder);
	OptimizedMITree(const Rule& r);
	OptimizedMITree();
	virtual ~OptimizedMITree() override;
	void Insertion(const Rule& rule);
	void Insertion(const Rule& rule, bool& priorityChange);
	bool TryInsertion(const Rule& rule, bool& priorityChange);

	void Deletion(const Rule& rule, bool& priorityChange);
	bool CanInsertRule(const Rule& r) const;
	void Print() const;
	void PrintFieldOrder() const;
	int ClassifyAPacket(const Packet& one_packet) const;
	int ClassifyAPacket(const Packet& one_packet, int priority_so_far) const;

	size_t NumRules() const;
	int MaxPriority() const;
	bool Empty() const;

	void ReconstructIfNumRulesLessThanOrEqualTo(int threshold = 10);
	std::vector<Rule> SerializeIntoRules() const;
	std::vector<Rule> GetRules() const;

	int MemoryConsumption() const;

	Memory MemSizeBytes(Memory ruleSize) const;

private:
	bool isMature = false;
	RedBlackTree * root;
	int numRules = 0;
	std::vector<int> fieldOrder;
	std::multiset<int> priorityContainer;
	int maxPriority = -1;

	bool IsIdenticalVector(const std::vector<int>& lhs,
			const std::vector<int>& rhs);

	void Reset();
};
