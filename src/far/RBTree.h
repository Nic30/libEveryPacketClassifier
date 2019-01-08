//#pragma once
//#include <iostream>
//
//
//enum Color {
//	RED, BLACK, DOUBLE_BLACK
//};
//
//template<typename KEY_t>
//struct RBNode {
//	RBNode *left, *right, *parent;
//	Color color;
//	KEY_t key;
//
//	explicit RBNode(KEY_t);
//};
//
///*
// * Red Black Tree
// * */
//template<typename KEY_t>
//class RBTree {
//public:
//	using Node_t = RBNode<KEY_t>;
//private:
//	Node_t *root;
//	size_t _M_size;
//protected:
//	void rotateLeft(Node_t *&);
//	void rotateRight(Node_t *&);
//
//	void fixAddRBTree(Node_t *&);
//	void fixDiscardRBTree(Node_t *&);
//
//	void inorderBST(Node_t *&);
//	//void preorderBST(Node_t *&);
//
//	// get color with nullptr check
//	int getColor(Node_t *&);
//	// set color with nullptr check
//	void setColor(Node_t *&, Color);
//	Node_t* minValueNode(Node_t *&);
//	Node_t* maxValueNode(Node_t *&);
//	/*
//	 * Add new key into node
//	 *
//	 * :param new_key: key to insert
//	 * :param new_node: ref. pointer for newly generated node for key
//	 * :param existing_node: ref. pointer for already present node for key
//	 * :return: nullptr if key was already present or pointer on new root
//	 **/
//	Node_t* addBST(Node_t *& root, const KEY_t & new_key,
//			Node_t *& new_node, Node_t *& existing_node);
//	Node_t* discardBST(Node_t *& root, KEY_t key);
//	int getBlackHeight(Node_t *);
//
//	static void printNodeName(std::ostream & str, size_t node_i) {
//		str << "node" << node_i;
//	}
//
//	static void printConnection(std::ostream & str, size_t node_a, size_t node_b) {
//		printNodeName(str, node_a);
//		str << " -- ";
//		printNodeName(str, node_b);
//		str << ";" << std::endl;
//	}
//
//	static void printNodeAttrs(std::ostream & str, size_t node_i, Node_t * node) {
//		printNodeName(str, node_i);
//		str << " [label=\"" << node->key << "\"];" << std::endl;
//		printNodeName(str, node_i);
//		str << " [shape=box];" << std::endl;
//		if (node->color == RED) {
//			printNodeName(str, node_i);
//			str <<  " [color=red];" << std::endl;
//		}
//	}
//
//	static size_t printNode(std::ostream & str, size_t node_i, Node_t * node) {
//		if (node == nullptr)
//			return 0;
//		printNodeAttrs(str, node_i, node);
//		size_t my_i = node_i;
//		node_i++;
//		size_t last_i = my_i;
//
//		if (node->left) {
//			node_i = printNode(str, last_i + 1, node->left);
//			printConnection(str, my_i, last_i + 1);
//			last_i = node_i;
//		}
//		if (node->right) {
//			node_i = printNode(str, last_i + 1, node->right);
//			printConnection(str, my_i, last_i + 1);
//			last_i = node_i;
//		}
//		return node_i;
//	}
//public:
//	RBTree();
//	RBTree(std::vector<KEY_t> &);
//	/*
//	 * :return: pointer of existing node if tree already contains the value else nullptr
//	 **/
//	bool add(const KEY_t &);
//
//	/*
//	 * :return: true if item was in tree
//	 **/
//	bool discard(const KEY_t &);
//
//	/*
//	 * Merge second tree to this tree
//	 * :note: Items are removed from second tree
//	 * */
//	void merge(RBTree &);
//
//	Node_t* search(KEY_t&);
//
//	size_t size();
//
//	// serialize graph to string in dot format
//	friend std::ostream & operator<<(std::ostream & str, const RBTree<KEY_t> & t) {
//		str <<  "graph rbtree {" << std::endl;
//		printNode(str, 0, t.root);
//		str << "}";
//
//		return str;
//	}
//
//	operator std::string() const {
//		std::stringstream ss;
//		ss << *this;
//		return ss.str();
//	}
//};
//
//#include "RBTree_impl.h"
//
