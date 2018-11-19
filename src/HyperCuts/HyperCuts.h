#pragma once

#include "../Simulation.h"
#include "../Trees/TreeUtils.h"

class HyperCutsNode {
public:
	int depth = 0;
	bool isProblematic = false;

	std::list<Rule*> classifier;
	std::list<HyperCutsNode*> children;
	std::vector<HyperCutsNode*> childArray;

	std::vector<Range1d> bounds;
	std::vector<int> cuts;
};

class HyperCutsHelper {
public:
	HyperCutsNode* CreateTree(const std::list<Rule*>& classifier);

private:
	std::list<HyperCutsNode*> CalcCuts(HyperCutsNode* node);
	std::list<HyperCutsNode*> CalcNumCuts1D(HyperCutsNode* root, size_t dim);
	std::list<HyperCutsNode*> CalcNumCuts2D(HyperCutsNode* root, size_t* dims);
	void CalcDimensionsToCut(HyperCutsNode* node, std::vector<bool>& selectDims);

	size_t leafSize = 8;
	double spfac = 4;

	bool isHyperCuts = true;

	bool compressionOn = false;
	bool binningOn = false;
	bool mergingOn = false;
};

class HyperCuts : public PacketClassifier {
public:
	virtual void ConstructClassifier(const std::vector<Rule>& rules);
	virtual int ClassifyAPacket(const Packet& packet);
	virtual void DeleteRule(size_t index) { 
		fprintf(stderr, "Can't delete rules.\n");
		exit(EXIT_FAILURE);
	}
	virtual void InsertRule(const Rule& rule) { 
		fprintf(stderr, "Can't insert rules.\n");
		exit(EXIT_FAILURE);
	}
	virtual Memory MemSizeBytes() const;
	virtual int MemoryAccess() const { return 0; } // TODO
	virtual size_t NumTables() const;
	virtual size_t RulesInTable(size_t tableIndex) const { return rules.size(); }

	virtual ~HyperCuts() {
	}
private:
	std::vector<Rule> rules;

	HyperCutsNode* root;
};
