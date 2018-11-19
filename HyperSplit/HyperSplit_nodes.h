#pragma once

#include "../Simulation.h"
#include "HyperSplit_iNode.h"



class NodeTable: public ClassifierTable {
public:
	NodeTable(iNode* root, int numRules, int maxPriority, unsigned int leafSize) :
			root(root), numRules(numRules), maxPriority(maxPriority), leafSize(
					leafSize) {
	}
	virtual int ClassifyAPacket(const Packet& packet) const {
		return root->ClassifyAPacket(packet);
	}
	virtual void Insertion(const Rule& rule, bool& priorityChange) {
		if (rule.priority > maxPriority) {
			maxPriority = rule.priority;
			priorityChange = true;
		} else {
			priorityChange = false;
		}
		root->InsertRule(leafSize, rule);
	}
	virtual void Deletion(const Rule& rule, bool& priorityChange) {
		priorityChange = false;
		root->DeleteRule(rule);
	}

	virtual bool CanInsertRule(const Rule& rule) const {
		return true;
	}

	virtual size_t NumRules() const {
		return numRules;
	}
	virtual int MaxPriority() const {
		return maxPriority;
	}

	virtual Memory MemSizeBytes(Memory ruleSize) const {
		return root->Size(ruleSize);
	}

	virtual std::vector<Rule> GetRules() const {
		return std::vector<Rule>(); // TODO
	}

private:
	iNode* root;
	size_t numRules;
	int maxPriority;
	unsigned int leafSize;
};

class SplitNode: public iNode {
public:
	/**
	 * Constructor: Takes ownership of the designated pointers
	 */
	SplitNode(const std::vector<Range1d>& bounds, unsigned int split, int dim,
			iNode* lc, iNode* rc) :
			splitPoint(split), splitDim(dim), leftChild(lc), rightChild(rc), bounds(
					bounds) {
		if (splitDim > 10 || splitDim < 0) {
			std::cout << splitDim << std::endl;
			throw "Bad dim in constructor!";
		}
		//CheckBounds(this->bounds, "Split-Const");
	}
	virtual ~SplitNode() {
		delete leftChild;
		delete rightChild;
	}

	int ClassifyAPacket(const Packet& p);
	iNode* DeleteRule(const Rule& r);
	iNode* InsertRule(unsigned int leafSize, const Rule& r);
	int Size(int ruleSize) const;
	bool IsEmpty() const {
		return false;
	}

	void SetBounds(const std::vector<Range1d>& bounds);
private:

	const unsigned int splitPoint;
	const int splitDim;
	iNode* leftChild;
	iNode* rightChild;

	std::vector<Range1d> bounds;
};

class ListNode: public iNode {
public:
	ListNode(const std::vector<Range1d>& bounds, const std::vector<Rule>& rules) :
			rules(rules), bounds(bounds) {
	}

	int ClassifyAPacket(const Packet& p);
	iNode* DeleteRule(const Rule& r);
	iNode* InsertRule(unsigned int leafSize, const Rule& r);
	int Size(int ruleSize) const;
	bool IsEmpty() const {
		return rules.empty();
	}
	void SetBounds(const std::vector<Range1d>& bounds);
private:

	std::vector<Rule> rules;
	std::vector<Range1d> bounds;
};

iNode* SplitRules(const std::vector<Range1d>& bounds,
		const std::vector<Rule>& rules, unsigned int leafSize);
