/*
 * MIT License
 *
 * Copyright (c) 2017 by J. Daly at Michigan State University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include "ByteCutsNode.h"
#include "TreeBuilder.h"
#include "../Simulation.h"
#include "../ElementaryClasses.h"

class ByteCutsClassifier: public PacketClassifier {
public:
	ByteCutsClassifier(const std::unordered_map<std::string, std::string>& args);
	ByteCutsClassifier(double dredgeFraction = 0.02, double turningPoint = 0.01, double minFrac = 0.75);
	~ByteCutsClassifier();

	virtual void ConstructClassifier(const std::vector<Rule>& rules);
	virtual int ClassifyAPacket(const Packet& packet);
	virtual void DeleteRule(size_t index) {
		printf("Can't delete rules.\n");
		exit(EXIT_FAILURE);
	}
	virtual void InsertRule(const Rule& rule) {
		printf("Can't insert rules.\n");
		exit(EXIT_FAILURE);
	}
	Memory MemSizeBytes() const {
		Memory mem = 0;
		int rulesize = 19;
		for (const ByteCutsNode* t : trees) {
			mem += t->Size(rulesize);
		}
		return mem;
	}
	virtual int MemoryAccess() const { return 0; } // TODO
	virtual size_t NumTables() const {
		return trees.size();
	}
	size_t NumGoodTrees() const {
		return goodTrees;
	}
	size_t NumBadTrees() const {
		return badTrees;
	}
	virtual size_t RulesInTable(size_t tableIndex) const {
		return sizes[tableIndex];
	}
	size_t PriorityOfTable(size_t tableIndex) const {
		return priorities[tableIndex];
	}
	
	int HeightOfTree(size_t tableIndex) const {
		return trees[tableIndex]->Height();
	}
	int CostOfTree(size_t tableIndex) const {
		return trees[tableIndex]->Cost();
	}
private:
	bool IsWideAddress(Range1dWeighted s) const;
	void BuildTree(const std::vector<Rule>& rules);
	void BuildBadTree(const std::vector<Rule>& rules);
	std::vector<Rule> Separate(const std::vector<Rule>& rules, std::vector<Rule>& remain);

	std::vector<Rule> rules;
	std::vector<ByteCutsNode*> trees;
	std::vector<int> priorities;
	std::vector<size_t> sizes;
	
	double dredgeFraction;
	double turningPoint;
	double minFrac;
	size_t goodTrees = 0;
	size_t badTrees = 0;
};
