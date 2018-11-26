#include "OptimizedMITree.h"

OptimizedMITree::OptimizedMITree(const SortableRuleset& rules) {
	numRules = 0;
	root = new RedBlackTree();
	fieldOrder = rules.GetFieldOrdering();
	maxPriority = -1;
	for (const auto& r : rules.GetRule()) {
		bool priorityChange;
		Insertion(r, priorityChange);
	}

}
OptimizedMITree::OptimizedMITree(const std::vector<int>& fieldOrder) :
		fieldOrder(fieldOrder) {
	root = new RedBlackTree();
	numRules = 0;
	maxPriority = -1;
}
OptimizedMITree::OptimizedMITree(const Rule& r) {
	root = new RedBlackTree();
	numRules = 0;
	fieldOrder = SortableRulesetPartitioner::GetFieldOrderByRule(r);
	maxPriority = -1;
}
OptimizedMITree::OptimizedMITree() {
	numRules = 0;
	root = new RedBlackTree();
	fieldOrder = {0, 1, 2, 3};
	maxPriority = -1;
}
OptimizedMITree::~OptimizedMITree() {
	delete root;
}
void OptimizedMITree::Insertion(const Rule& rule) {
	priorityContainer.insert(rule.priority);
	maxPriority = std::max(maxPriority, rule.priority);
	root->insertWithPathCompression(rule.range, 0, fieldOrder, rule.priority);
	numRules++;
	counter++;
}
void OptimizedMITree::Insertion(const Rule& rule, bool& priorityChange) {
	//	if (CanInsertRule(rule)) {
	priorityContainer.insert(rule.priority);
	priorityChange = rule.priority > maxPriority;
	maxPriority = std::max(maxPriority, rule.priority);
	root->insertWithPathCompression(rule.range, 0, fieldOrder, rule.priority);
	numRules++;
	counter++;
	//		}
}
bool OptimizedMITree::TryInsertion(const Rule& rule, bool& priorityChange) {
	if (CanInsertRule(rule)) {
		counter++;
		priorityContainer.insert(rule.priority);
		priorityChange = rule.priority > maxPriority;
		maxPriority = std::max(maxPriority, rule.priority);
		root->insertWithPathCompression(rule.range, 0, fieldOrder,
				rule.priority);
		numRules++;
		return true;
	} else {
		return false;
	}
}

void OptimizedMITree::Deletion(const Rule& rule, bool& priorityChange) {
	auto pit = priorityContainer.equal_range(rule.priority);

	priorityContainer.erase(pit.first);

	if (numRules == 1) {
		maxPriority = -1;
		priorityChange = true;
	} else if (rule.priority == maxPriority) {
		priorityChange = true;
		maxPriority = *priorityContainer.rbegin();
	}
	numRules--;
	bool JustDeletedTree;
	RedBlackTree::deleteWithPathCompression(root, rule.range, 0, fieldOrder,
			rule.priority, JustDeletedTree);
}
bool OptimizedMITree::CanInsertRule(const Rule& r) const {
	return root->canInsert(r.range, 0, fieldOrder);
}

void OptimizedMITree::Print() const {
	root->print();
}

void OptimizedMITree::PrintFieldOrder() const {
	for (size_t i = 0; i < fieldOrder.size(); i++) {
		printf("%d ", fieldOrder[i]);
	}
	printf("\n");
}

int OptimizedMITree::ClassifyAPacket(const Packet& one_packet) const {
	return root->exactQueryIterative(one_packet, fieldOrder);
	//	return   RBExactQuery(root, one_packet, 0,fieldOrder);
}

int OptimizedMITree::ClassifyAPacket(const Packet& one_packet,
		int priority_so_far) const {
	return root->exactQueryPriority(one_packet, 0, fieldOrder, priority_so_far);
}

size_t OptimizedMITree::NumRules() const {
	return numRules;
}
int OptimizedMITree::MaxPriority() const {
	return maxPriority;
}
bool OptimizedMITree::Empty() const {
	return priorityContainer.empty();
}

void OptimizedMITree::ReconstructIfNumRulesLessThanOrEqualTo(
		int threshold = 10) {
	if (isMature)
		return;

	if (numRules >= threshold) {
		isMature = true;
		return;
	}
	//global_counter++;
	std::vector<Rule> serialized_rules = SerializeIntoRules();
	auto result =
			SortableRulesetPartitioner::FastGreedyFieldSelectionForAdaptive(
					serialized_rules);
	if (!result.first)
		return;

	if (IsIdenticalVector(fieldOrder, result.second))
		return;

	Reset();
	fieldOrder = result.second;
	for (const auto & r : serialized_rules) {
		Insertion(r);
	}
}
std::vector<Rule> OptimizedMITree::SerializeIntoRules() const {
	return root->serializeIntoRules(fieldOrder);
}
std::vector<Rule> OptimizedMITree::GetRules() const {
	return SerializeIntoRules();
}

int OptimizedMITree::MemoryConsumption() const {
	return root->calculateMemoryConsumption(fieldOrder);
}

Memory OptimizedMITree::MemSizeBytes(Memory ruleSize) const {
	return MemoryConsumption();
}

bool OptimizedMITree::IsIdenticalVector(const std::vector<int>& lhs,
		const std::vector<int>& rhs) {
	for (size_t i = 0; i < lhs.size(); i++) {
		if (lhs[i] != rhs[i])
			return false;
	}
	return true;
}

void OptimizedMITree::Reset() {
	delete root;
	numRules = 0;
	fieldOrder.clear();
	priorityContainer.clear();
	maxPriority = -1;
	root = new RedBlackTree();
}
