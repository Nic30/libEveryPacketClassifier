#pragma once
#include <stdint.h>
#include <array>

template<size_t DIM_CNT>
class CompressedDecisionTree {
public:
	struct Node {
		uint16_t val;
		uint16_t rule;
		uint8_t dim;
		uint8_t __padding__;
	}__attribute__((PACKED));
	Node * root;

	CompressedDecisionTree(const CompressedDecisionTree & other) = delete;
	CompressedDecisionTree(size_t max_rule_cnt) {
		root = new Node[max_rule_cnt];
	}

	/*
	 * :return: rule index (0 is reserved for no rule)
	 **/
	constexpr uint16_t classify(const std::array<uint16_t, DIM_CNT> & value,
			uint16_t level = 0, uint16_t node_index = 0) {

		while (true) {
			const auto & n = root[node_index];
			if (value[n.dim] <= n.value) {

			} else {

			}
			return n.rule;
		}
	}
	~CompressedDecisionTree() {
		if (root)
			delete[] root;
	}
};

