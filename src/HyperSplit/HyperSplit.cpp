#include <iostream>
#include <set>
#include <algorithm>
#include <utility>
#include <string>

#include "HyperSplit.h"
#include "HyperSplit_nodes.h"

using namespace std;

void HyperSplit::_ConstructClassifier(const vector<Rule>& rules) {
	this->rules = rules;
	root = SplitRules(bounds, this->rules, leafSize);
}

int HyperSplit::ClassifyAPacket(const Packet& p) {
	return root->ClassifyAPacket(p);
}

void HyperSplit::DeleteRule(size_t index) {
	//CheckBounds(bounds);

	swap(rules[index], rules[rules.size() - 1]);
	iNode* r = root->DeleteRule(rules[rules.size() - 1]);
	if (r != root) {
		delete root;
		root = r;
	}
	rules.pop_back();
}

void HyperSplit::InsertRule(const Rule& r) {
	//CheckBounds(bounds);

	rules.push_back(r);
	iNode* n = root->InsertRule(leafSize, r);
	if (n != root) {
		delete root;
		root = n;
	}
}

Memory HyperSplit::MemSizeBytes() const {
	int size = 0;
	for (const auto & field : bounds) {
		if (field.high == 0xFFFFFFFFu) {
			size += 5;
		} else if (field.high == 0xFFu) {
			size += 1;
		} else { //if (field.high == 0xFFFFu) {
			size += 4;
		}
		// else {
		// throw std::exception("Unknown bounds");
		// }
	}
	return root->Size(size);
}
