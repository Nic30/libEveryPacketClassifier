#pragma once
#include <iostream>

/**    Left-Leaning Red-Black (LLRB) is an implementation of
  *  2–3 balanced binary search tree.
  *  Ported to Python from code and description on
  *  paper “Left-leaning Red-Black Trees” by Robert Sedgewick:
  *  http://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
  */
template<typename KEY_t>
class RBTree {
public:
	// forward declarations
	enum Color_t {
		RED, BLACK
	};
	struct Node_t;

private:
	inline bool is_red(Node_t * node) {
		return node && node->color == BLACK;
	}
	inline bool is_black(Node_t * node) {
		return !is_red(node);
	}
public:
	struct Node_t {
		Node_t *left, *right;
		Color_t color;
		size_t height;
		KEY_t key;

		explicit Node_t(const KEY_t & key) :
				left(nullptr), right(nullptr), color(RED), height(1), key(key) {
		}
		// Search the subtree for a key.
		//  :return: value or nullptr
		Node_t * find(const KEY_t & key) {
			if (key == this->key) {
				return this;
			} else if (key < this->key && left) {
				return left->find(key);
			} else if (this->key < key && right) {
				return right->find(key);
			}
			return nullptr;
		}

		/*
		 * insert to binary tree
		 * :return: pair of <node reference for key, true if new node was created else false>
		 * */
		std::pair<Node_t &, bool> insert_bin_tree(const KEY_t & key) {
			if (this->key == key) {
				return {*this, false};
			} else if (key < this->key) {
				if (left) {
					return left->insert_bin_tree(key);
				} else {
					left = new Node_t(key);
					return {*left, true};
				}
			} else {
				if (right) {
					return right->insert_bin_tree(key);
				} else {
					right = new Node_t(key);
					return {*right, true};
				}
			}
		}

		/*
		 * insert to llrb tree
		 * :return: pair of <node reference for key, true if new node was created else false>
		 * */
		std::pair<Node_t &, bool> insert(const KEY_t & key) {
			if(is_red(this->left) && is_red(right)) {

			}
		}

		Node_t & min() {
			if (left)
				return left->min();
			return *this;
		}
		Node_t & max() {
			if (right)
				return right->max();
			return *this;
		}
	};
private:
	Node_t *root;
	size_t _M_size;
protected:

	/*
	 * Add new key into node
	 *
	 * :param new_key: key to insert
	 * :param new_node: ref. pointer for newly generated node for key
	 * :param existing_node: ref. pointer for already present node for key
	 * :return: nullptr if key was already present or pointer on new root
	 **/

	static void printNodeName(std::ostream & str, size_t node_i) {
		str << "node" << node_i;
	}

	static void printConnection(std::ostream & str, size_t node_a,
			size_t node_b) {
		printNodeName(str, node_a);
		str << " -- ";
		printNodeName(str, node_b);
		str << ";" << std::endl;
	}

	static void printNodeAttrs(std::ostream & str, size_t node_i,
			Node_t * node) {
		printNodeName(str, node_i);
		str << " [label=\"" << node->key << "\"];" << std::endl;
		printNodeName(str, node_i);
		str << " [shape=box];" << std::endl;
		if (node->color == RED) {
			printNodeName(str, node_i);
			str << " [color=red];" << std::endl;
		}
	}

	static size_t printNode(std::ostream & str, size_t node_i, Node_t * node) {
		if (node == nullptr)
			return 0;
		printNodeAttrs(str, node_i, node);
		size_t my_i = node_i;
		node_i++;
		size_t last_i = my_i;

		if (node->left) {
			node_i = printNode(str, last_i + 1, node->left);
			printConnection(str, my_i, last_i + 1);
			last_i = node_i;
		}
		if (node->right) {
			node_i = printNode(str, last_i + 1, node->right);
			printConnection(str, my_i, last_i + 1);
			last_i = node_i;
		}
		return node_i;
	}


public:
	RBTree(): root(nullptr), _M_size(0) {
	}
	/*
	 * :return: pair <reference to node, true if new node was created else false>
	 **/
	std::pair<Node_t &, bool> insert(const KEY_t & key) {
		if (root) {
			return root->insert(key);
		} else {
			root = new Node_t(key);
			return {*root, true};
		}
	}

	/*
	 * :return: true if item was in tree
	 **/
	bool discard(const KEY_t &);

	/*
	 * Merge second tree to this tree
	 * :note: Items are removed from second tree
	 * */
	void merge(RBTree &);

	Node_t* find(const KEY_t & key) {
		if (root) {
			root->find(key);
		}
		return nullptr;
	}

	size_t size();

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const RBTree<KEY_t> & t) {
		str << "graph rbtree {" << std::endl;
		printNode(str, 0, t.root);
		str << "}";

		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};

#include "RBTree_impl.h"

