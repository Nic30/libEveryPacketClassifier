#pragma once
#include <stdint.h>
#include <array>
#include <limits>

/*
 * :param DIM_T: type to represent the dimension index
 * :param DIM_CNT: number of dimensions
 **/
template<size_t DIM_CNT>
class CompressedDecisionTree {
public:
	/*
	 * Node of binary tree with range as a key and rule as a value
	 * */
	template<size_t DIM_CNT>
	struct Node {
		uint16_t rule;
		uint8_t dim_offset;
		uint8_t dim_cnt;
		// lower and upper constrain
		// values ordered by specified field_order
		uint16_t val[DIM_CNT * 2];
	};
	// __attribute__((PACKED))
	using MaxNode = Node<DIM_CNT>;
	using value_type = const std::array<uint16_t, DIM_CNT>;

	std::array<uint8_t, DIM_CNT> field_order;
	MaxNode * root __attribute__((aligned(64)));

	CompressedDecisionTree(const CompressedDecisionTree & other) = delete;
	/*
	 * :param max_node_cnt: maximum count of nodes for memory preallocation
	 * :param field_order: filed order which is used by tree
	 ***/
	CompressedDecisionTree(size_t max_node_cnt,
			std::array<uint8_t, DIM_CNT> field_order) :
			field_order(field_order) {
		static_assert(max_node_cnt <= std::numeric_limits<uint16_t>::max());
		static_assert(DIM_CNT <= std::numeric_limits<uint8_t>::max());

		// check if permutation in field_order is correct
		for (uint8_t i = 0; i < DIM_CNT; i++) {
			bool found = false;
			for (auto item : field_order) {
				if (i == item) {
					found = true;
					break;
				}
			}
			if (!found)
				throw std::runtime_error(
						"field order permutation is not correct");
		}

		root = new MaxNode[max_node_cnt];
	}

	template<typename cmp>
	static constexpr bool array_cmp(const uint16_t * a, const uint16_t * b,
			uint8_t size) {
		// [TODO] AVX https://software.intel.com/en-us/node/534476
		bool res = true;
		for (uint8_t i = 0; i < size; i++)
			res &= cmp()(a[i], b[i]);

		return res;
	}

	static constexpr value_type apply_field_order(const value_type & val) {
		value_type res;
		for (size_t i = 0; i < DIM_CNT; i++) {
			res[i] = val[field_order[i]];
		}
		return res;
	}
	/*
	 * :return: rule index (0 is reserved for no rule)
	 **/
	constexpr uint16_t classify(const value_type & _value, uint16_t node_index =
			0) {
		auto value = apply_field_order(_value);
		while (true) {
			const auto & n = root[node_index];
			if (array_cmp<std::less>(value, n.val, n.dim_cnt)) {
				// go left for lower values
				node_index = 2 * node_index + 1;
				continue;
			} else if (array_cmp<std::greater_equal>(value, n.val + n.dim_cnt,
					n.dim_cnt)) {
				// go right for higher values
				node_index = 2 * node_index + 2;
				continue;
			} else {
				// this is rule contains the result of classification
				return n.rule;
			}
		}
	}
	~CompressedDecisionTree() {
		if (root)
			delete[] root;
	}
}__attribute__((aligned(64)));

