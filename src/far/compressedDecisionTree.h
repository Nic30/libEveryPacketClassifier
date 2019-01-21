#pragma once
#include <stdint.h>
#include <array>
#include <limits>
#include <functional>

class ArrayBinTreeUtils {
public:
	template<typename T>
	static constexpr T get_left_index(T node_index) {
		return 2 * node_index + 1;
	}

	template<typename T>
	static constexpr T get_right_index(T node_index) {
		return 2 * node_index + 2;
	}
};

template<typename Node_t>
class CompressedDecisionTree_nodeprint: ArrayBinTreeUtils {
public:
	static void printNodeName(std::ostream & str, size_t node_i) {
		str << "node" << node_i;
	}

	static void printConnection(std::ostream & str, size_t node_a,
			size_t node_b, const std::string & label) {
		printNodeName(str, node_a);
		str << " -- ";
		printNodeName(str, node_b);
		str << " [label=\"" << label << "\"]";
		str << ";" << std::endl;
	}

	static void printNodeAttrs(std::ostream & str, size_t node_i,
			Node_t * root) {
		auto * node = &root[node_i];
		printNodeName(str, node_i);
		str << " [label=\"";
		auto of = node->dim_offset;
		uint8_t last_i = node->dim_cnt - 1;
		str << "{" << size_t(node->rule) << "} ";
		for (size_t i = 0; i < node->dim_cnt; i++) {
			str << "[" << (i + of) << "]<=" << size_t(node->val[i]);
			if (i != last_i)
				str << ",";
		}
		str << "...";
		for (size_t i = 0; i < node->dim_cnt; i++) {
			str << "[" << (i + of) << "]<=" << size_t(node->val[i + node->dim_cnt]);
			if (i != last_i)
				str << ",";
		}
		str << "\"];" << std::endl;
		printNodeName(str, node_i);
		str << " [shape=box];" << std::endl;
	}

	static void printNode(std::ostream & str, size_t node_i, Node_t * root) {
		printNodeAttrs(str, node_i, root);
		auto * node = &root[node_i];

		if (node->left_vld) {
			auto left_i = get_left_index(node_i);
			printNode(str, left_i, root);
			printConnection(str, node_i, left_i, "L");
		}
		if (node->right_vld) {
			auto right_i = get_right_index(node_i);
			printNode(str, right_i, root);
			printConnection(str, node_i, right_i, "R");
		}
	}
};

/*
 * :param DIM_T: type to represent the dimension index
 * :param DIM_CNT: number of dimensions
 **/
template<size_t DIM_CNT, typename rule_id_t = uint16_t,
		typename node_value_t = uint16_t>
class CompressedDecisionTree: public ArrayBinTreeUtils {
public:
	/*
	 * Node of binary tree with range as a key and rule as a value
	 * */
	template<size_t _DIM_CNT>
	struct Node {
		rule_id_t rule;
		uint8_t dim_offset :7; // number of skipped dimensions from the beginning of value
		uint8_t left_vld :1; // has left child flag
		uint8_t dim_cnt :7; // number of dimension check in this node
		uint8_t right_vld :1; // has right child flag

		// lower and upper constrain
		// values ordered by specified field_order
		node_value_t val[_DIM_CNT * 2];
	};
	// __attribute__((PACKED))
	static constexpr rule_id_t INVALID_RULE = 0;
	using MaxNode = Node<DIM_CNT>;
	using value_type = std::array<node_value_t, DIM_CNT>;

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
		assert(max_node_cnt <= std::numeric_limits<uint16_t>::max());
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

	// a < b
	static bool array_less(const node_value_t * a, const node_value_t * b,
			uint8_t size) {
		// [TODO] AVX https://software.intel.com/en-us/node/534476
		for (unsigned i = 0; i < size; i++)
			if (a[i] == b[i])
				continue;
			else
				return a[i] < b[i];

		// equal
		return false;
	}

	// a > b
	static bool array_greater(const node_value_t * a,
			const node_value_t * b, uint8_t size) {
		// [TODO] AVX https://software.intel.com/en-us/node/534476
		for (unsigned i = 0; i < size; i++)
			if (a[i] == b[i])
				continue;
			else
				return a[i] > b[i];

		// equal
		return false;
	}

	constexpr void apply_field_order(const value_type & val, value_type & res) {
		for (unsigned i = 0; i < DIM_CNT; i++) {
			res[i] = val[field_order[i]];
		}
	}

	/*
	 * :return: rule index (0 is reserved for no rule)
	 **/
	rule_id_t classify(const value_type & _value) {
		value_type value_tmp;
		apply_field_order(_value, value_tmp);
		auto value = reinterpret_cast<const node_value_t*>(&value_tmp);
		uint16_t node_index = 0;

		while (true) {
			const auto & n = root[node_index];
			// [TODO] check validity before comparing
			if (array_less(value, &n.val[0], n.dim_cnt)) {
				if (not n.left_vld)
					return INVALID_RULE;
				// go left for lower values
				node_index = get_left_index(node_index);
			} else if (array_greater(value, &n.val[n.dim_cnt],
					n.dim_cnt)) {
				if (not n.right_vld)
					return INVALID_RULE;
				// go right for higher values
				node_index = get_right_index(node_index);
			} else {
				// this is rule contains the result of classification
				return n.rule;
			}
		}
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const CompressedDecisionTree & t) {
		str << "graph rbtree {" << std::endl;
		CompressedDecisionTree_nodeprint<MaxNode>::printNode(str, 0, t.root);
		str << "}";

		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

	~CompressedDecisionTree() {
		if (root)
			delete[] root;
	}

}__attribute__((aligned(64)));

