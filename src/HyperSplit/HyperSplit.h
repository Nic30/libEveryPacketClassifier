#pragma once

#include "../Simulation.h"
#include "HyperSplit_iNode.h"

class HyperSplit: public PacketClassifier {

public:
	HyperSplit(std::vector<Range1d> bounds, int leafSize) :
			bounds(bounds), root(nullptr), leafSize(leafSize) {
	}

	HyperSplit(int leafSize = 8) :
			root(nullptr), leafSize(leafSize) {
		bounds.push_back( { 0, 0xFFFFFFFFu });
		bounds.push_back( { 0, 0xFFFFFFFFu });
		bounds.push_back( { 0, 0xFFFFu });
		bounds.push_back( { 0, 0xFFFFu });
		bounds.push_back( { 0, 0xFFu });
	}
	HyperSplit(const std::unordered_map<std::string, std::string>& args) :
			HyperSplit(GetIntOrElse(args, "HyperSplit.Leaf", 8)) {
	}
	virtual ~HyperSplit() {
		delete root;
	}

	void ConstructClassifier(const std::vector<Rule>& rules);
	int ClassifyAPacket(const Packet& p);
	void DeleteRule(size_t index);
	void InsertRule(const Rule& r);
	Memory MemSizeBytes() const;
	int MemoryAccess() const {
		printf("warning unimplemented MemoryAccess()\n");
		return 0;
	}
	size_t NumTables() const {
		return 1;
	}
	size_t RulesInTable(size_t index) const {
		return rules.size();
	}
private:
	std::vector<Range1d> bounds;
	iNode* root;
	std::vector<Rule> rules;
	int leafSize;
};

