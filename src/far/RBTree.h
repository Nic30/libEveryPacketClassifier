#pragma once
#include <iostream>

template<typename Node_t, bool RED>
class LLRBTree_nodeprint {
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
			printConnection(str, my_i, last_i + 1, "L");
			last_i = node_i;
		}
		if (node->right) {
			node_i = printNode(str, last_i + 1, node->right);
			printConnection(str, my_i, last_i + 1, "R");
			last_i = node_i;
		}
		return node_i;
	}
};

/**  Left-Leaning Red-Black (LLRB) is an implementation of
 *  2–3 balanced binary search tree.
 *  “Left-leaning Red-Black Trees” by Robert Sedgewick:
 *  http://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
 */
template<typename KEY_t>
class LLRBTree {
public:
	// forward declarations
	using Color_t = bool;
	static constexpr Color_t RED = false;
	static constexpr Color_t BLACK = true;

	struct Node_t;

private:
	inline static bool is_red(Node_t * node) {
		return node && node->color == BLACK;
	}
	inline static bool is_black(Node_t * node) {
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
		Node_t & insert_bin_tree(const KEY_t & key) {
			if (this->key == key) {
				// key already present in tree
				throw std::runtime_error("value already present in tree");
			} else if (key < this->key) {
				if (left) {
					left = &left->insert(key);
				} else {
					left = new Node_t(key);
				}
			} else {
				if (right) {
					right = &right->insert(key);
				} else {
					right = new Node_t(key);
				}
			}
			return *this;
		}

		/* Fix the Left-leaning Red-black tree properties
		 * with upto two rotations and a possible color flip.
		 */
		Node_t & _fix_up() {
			auto & _this = *this;
			if (is_red(right)) {
				_this = _rotate_left();
			}
			if (is_red(_this.left) and _this.left
					and is_red(_this.left->left)) {
				_this = _this._rotate_right();
			}

			if (is_red(_this.left) and is_red(_this.right)) {
				_this._flip_colors();
			}
			return _this._set_height();
		}
		// Flip colors to split a 4-node
		void _flip_colors() {
			color = not color;
			left->color = not left->color;
			right->color = not right->color;
		}

		/*
		 *  Assuming that self is red and both self.left and self.left.left
		 *  are black, make self.left or one of its children red.
		 */
		Node_t & _move_red_left() {
			_flip_colors();
			if (right and is_red(right->left)) {
				right = &right->_rotate_right();
				auto & _this = _rotate_left();
				_this._flip_colors();
				return _this;
			}
			return *this;
		}
		/* Assuming that self is red and both right and right.left
		 * are black, make right or one of its children red.
		 */
		Node_t & _move_red_right() {
			_flip_colors();
			if (left and is_red(left->left)) {
				auto & _this = _rotate_right();
				_this._flip_colors();
				return _this;
			}
			return *this;
		}
		/*
		 * Left rotate (right link of self)
		 *        V         |          V <--left or right, red or black
		 *        |         |          |
		 * out<--(x)   <<< LEFT       (s) <--in
		 *      // \        |         / \\  <--red
		 *    (s)   3       |        1   (x)
		 *    / \           |            / \
         *   1   2          |           2   3
		 */
		Node_t & _rotate_left() {
			auto x = right;
			right = x->left;
			x->left = this;
			x->color = color;
			color = RED;
			return *x;
		}

		/* Right rotate (left link of self)
		 *        V         |          V <--left or right, red or black
		 *        |         |          |
		 * in--> (s)     RIGHT >>>    (x)-->out
		 *      // \        |         / \\  <--red
		 *    (x)   3       |        1   (s)
		 *    / \           |            / \
         *   1   2          |           2   3
		 */
		Node_t & _rotate_right() {
			auto x = left;
			left = x->right;
			x->right = this;
			x->color = color;
			color = RED;
			return *x;
		}
		/*
		 * Recursively insert a node with key and optional value into the tree below.
		 */
		Node_t & insert(const KEY_t & key) {
			if (is_red(left) and is_red(right))
				_flip_colors();

			Node_t & _this = insert_bin_tree(key);

			if (is_red(_this.right) && is_black(_this.left))
				_this = _this._rotate_left();

			if (is_red(_this.left) && _this.left && is_red(_this.left->left))
				_this = _this._rotate_right();

			return _this._set_height();
		}

		/*    Delete the smallest node on the (left) subtree below
		 *   while maintaining balance.
		 */
		Node_t * _delete_min() {
			if (left == nullptr)
				return nullptr;

			Node_t & _this = *this;
			if (is_black(left) and left and is_black(left->left))
				_this = _this._move_red_left();

			_this.left = _this.left->_delete_min();

			return &_this._fix_up();
		}
		// Delete a node with the given key (recursively) from the tree below.
		static Node_t * _remove(Node_t * _this, KEY_t & k) {
			if (k < _this->key) {
				if (is_black(_this->left) and _this->left
						and is_black(_this->left->left))
					_this = &_this->_move_red_left();
				_this->left = _remove(_this->left, k);
			} else {
				if (is_red(_this->left))
					_this = &_this->_rotate_right();

				if (k == _this->key and !_this->right) {
					delete _this;
					return nullptr;
				}

				if (is_black(_this->right) and _this->right
						and is_black(_this->right->left))
					_this = &_this->_move_red_right();

				if (k == _this->key) {
					_this->key = _this->right->min().key;
					_this->right = _this->right->_delete_min();
				} else {
					if (_this->right == nullptr) {
						throw std::runtime_error(
								"item is not present in the tree and delete alg. may destroyed the tree");
					}
					_this->right = _remove(_this->right, k);
				}
			}
			return &_this->_fix_up();
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

		// Update height.
		Node_t & _set_height() {
			height = 1
					+ std::max(left ? left->height : 0,
							right ? right->height : 0);
			return *this;
		}
	};
private:
	Node_t *root;
	size_t _M_size;

public:
	LLRBTree() :
			root(nullptr), _M_size(0) {
	}
	/*
	 * :return: pair <reference to node, true if new node was created else false>
	 **/
	void insert(const KEY_t & key) {
		if (root) {
			root = &root->insert(key);
		} else {
			root = new Node_t(key);
		}
		root->color = BLACK;
		_M_size++;
	}

	// Delete a node with the given key from the tree.
	void remove(KEY_t & k) {
		if (!root)
			throw std::runtime_error("remove of item which is not in tree");

		if (is_black(root->left) and is_black(root->right))
			root->color = RED;
		root = root->_remove(root, k);
		if (root)
			root->color = BLACK;

		_M_size--;
	}

	/*
	 * Merge second tree to this tree
	 * :note: Items are removed from second tree
	 * */
	//void merge(LLRBTree &);
	Node_t* find(const KEY_t & key) {
		if (root) {
			root->find(key);
		}
		return nullptr;
	}

	size_t size() {
		return _M_size;
	}

	// serialize graph to string in dot format
	friend std::ostream & operator<<(std::ostream & str,
			const LLRBTree<KEY_t> & t) {
		str << "graph rbtree {" << std::endl;
		LLRBTree_nodeprint<Node_t, RED>::printNode(str, 0, t.root);
		str << "}";

		return str;
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
};
