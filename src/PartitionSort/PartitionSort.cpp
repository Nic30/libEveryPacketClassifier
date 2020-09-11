#include "PartitionSort.h"
#include "test_red_black_tree.h"
using namespace std;

void PartitionSort::ConstructClassifier(const vector<Rule>& rules) {
	//rb_selftest0();
	this->rules.reserve(rules.size());
	for (const auto& r : rules) {
		InsertRule(r);
	}
}

int PartitionSort::ClassifyAPacket(const Packet& packet) {
	int result = -1;
	int query = 0;
	for (const auto& t : mitrees) {
		if (result > t->MaxPriority()) {
			break;
		}
		query++;
		result = max(t->ClassifyAPacket(packet), result);
	}
	QueryCountersUpdate(query);
	return result;

}

Memory PartitionSort::MemSizeBytes() const {
	int size_total_bytes = 0;
	for (const auto& t : mitrees) {
		size_total_bytes += t->MemoryConsumption();
	}
	int size_array_pointers = mitrees.size();
	int size_of_pointer = 4;
	return size_total_bytes + size_array_pointers * size_of_pointer;
}

int PartitionSort::MemoryAccess() const {
	return 0;
}

size_t PartitionSort::NumTables() const {
	return mitrees.size();
}

size_t PartitionSort::RulesInTable(size_t index) const {
	return mitrees[index]->NumRules();
}

PartitionSort::~PartitionSort() {
	for (auto x : mitrees) {
		delete x;
	}
}

void PartitionSort::DeleteRule(size_t i) {
	if (i < 0 || i >= rules.size()) {
		cout << "Warning index delete rule out of bound: do nothing here"
				<< endl;
		cout << i << " vs. size: " << rules.size() << endl;
		return;
	}
	bool prioritychange = false;

	OptimizedMITree * mitree = rules[i].second;
	mitree->Deletion(rules[i].first, prioritychange);

	if (prioritychange) {
		InsertionSortMITrees();
	}

	if (mitree->Empty()) {
		mitrees.pop_back();
		delete mitree;
	}

	if (i != rules.size() - 1) {
		rules[i] = move(rules[rules.size() - 1]);
	}
	rules.pop_back();

}

void PartitionSort::InsertRule(const Rule& one_rule) {
	for (auto mitree : mitrees) {
		bool prioritychange = false;

		bool success = mitree->TryInsertion(one_rule, prioritychange);
		if (success) {
			if (prioritychange) {
				InsertionSortMITrees();
			}
			mitree->ReconstructIfNumRulesLessThanOrEqualTo(10);
			rules.push_back(make_pair(one_rule, mitree));
			return;
		}
	}
	bool priority_change = false;

	auto tree_ptr = new OptimizedMITree(one_rule);
	tree_ptr->TryInsertion(one_rule, priority_change);
	rules.push_back(make_pair(one_rule, tree_ptr));
	mitrees.push_back(tree_ptr);
	InsertionSortMITrees();
}

void PartitionSort::InsertionSortMITrees() {
	int i, j, numLength = mitrees.size();
	OptimizedMITree * key;
	for (j = 1; j < numLength; j++) {
		key = mitrees[j];
		for (i = j - 1; (i >= 0); i--) {
			if (!(mitrees[i]->MaxPriority() < key->MaxPriority()))
				break;
			mitrees[i + 1] = mitrees[i];
		}
		mitrees[i + 1] = key;
	}
}
