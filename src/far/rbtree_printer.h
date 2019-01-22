#pragma once

template<typename Node_t>
class RBTreePrinter {
public:
	virtual bool is_red(const Node_t & n) = 0;
	virtual void print_key(const Node_t & n, std::ostream & str) = 0;
	virtual const Node_t * left(const Node_t & n) = 0;
	virtual const Node_t * right(const Node_t & n) = 0;

	std::map<const Node_t*, size_t> node_to_id;

	void printNodeName(std::ostream & str, size_t node_i) {
		str << "node" << node_i;
	}

	void printConnection(std::ostream & str, size_t node_a, size_t node_b,
			const std::string & label) {
		printNodeName(str, node_a);
		str << " -- ";
		printNodeName(str, node_b);
		str << " [label=\"" << label << "\"]";
		str << ";" << std::endl;
	}

	void printNodeAttrs(std::ostream & str, size_t node_i,
			const Node_t & node) {
		printNodeName(str, node_i);
		str << " [label=\"";
		print_key(node, str);
		str << "\"];" << std::endl;
		printNodeName(str, node_i);
		str << " [shape=box];" << std::endl;
		if (is_red(node)) {
			printNodeName(str, node_i);
			str << " [color=red];" << std::endl;
		}
	}

	/*
	 * Recursively print the tree node by node (in order)
	 *
	 * :param str: the output stream where to print
	 * :param node_i: index of this node (used as unique id)
	 * :param node: the root of tree to print
	 * :return the number of nodes in tree
	 **/
	size_t printNode(std::ostream & str, size_t node_i, const Node_t * node) {
		if (node == nullptr)
			return 0;
		node_to_id[node] = node_i;
		printNodeAttrs(str, node_i, *node);
		size_t my_i = node_i;
		node_i++;
		size_t last_i = my_i;

		if (left(*node)) {
			node_i = printNode(str, last_i + 1, left(*node));
			printConnection(str, my_i, last_i + 1, "L");
			last_i = node_i;
		}
		if (right(*node)) {
			node_i = printNode(str, last_i + 1, right(*node));
			printConnection(str, my_i, last_i + 1, "R");
			last_i = node_i;
		}
		return node_i;
	}

	virtual ~RBTreePrinter() {
	}
};
