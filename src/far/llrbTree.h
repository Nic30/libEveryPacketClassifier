#pragma once
#include <iostream>
#include <algorithm>
#include <functional>

#include <set>
enum class LLRB_NODE {
	BST, TD234, BU23
};

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
template<typename KEY_t, typename eq = std::equal_to<KEY_t>,
		typename less = std::less_equal<KEY_t>, LLRB_NODE species =
				LLRB_NODE::TD234>
class LLRBTree {
public:
	// forward declarations
	using Color_t = bool;
	static constexpr Color_t RED = false;
	static constexpr Color_t BLACK = true;

	using Key_t = KEY_t;

	struct Node_t;

private:
	inline static bool isRed(Node_t * node) {
		return node and node->color == RED;
	}
	inline static bool isBlack(Node_t * node) {
		return !isRed(node);
	}
public:
	/*
	 * :ivar N: number of nodes in this tree
	 * */
	struct Node_t {
		Node_t *left, *right;
		Color_t color;
		size_t height, N;
		KEY_t key;

		explicit Node_t(const KEY_t & key) :
				left(nullptr), right(nullptr), color(RED), height(1), N(1), key(
						key) {
		}
		// Search the subtree for a key.
		//  :return: value or nullptr
		//Node_t * find(const KEY_t & key) {
		//	if (key == this->key) {
		//		return this;
		//	} else if (key < this->key and left) {
		//		return left->find(key);
		//	} else if (this->key < key and right) {
		//		return right->find(key);
		//	}
		//	return nullptr;
		//}


		// Flip colors to split a 4-node
		void _flip_colors() {
			color = not color;
			left->color = not left->color;
			right->color = not right->color;
		}

		//
		///*    Delete the smallest node on the (left) subtree below
		// *   while maintaining balance.
		// */
		//Node_t * _delete_min() {
		//	if (left == nullptr)
		//		return nullptr;
		//
		//	Node_t & _this = *this;
		//	if (isBlack(left) and left and isBlack(left->left))
		//		_this = _this.moveRedLeft();
		//
		//	_this.left = _this.left->_delete_min();
		//
		//	return &_this.fixUp();
		//}
		//// Delete a node with the given key (recursively) from the tree below.
		//static Node_t * _remove(Node_t * _this, KEY_t & k) {
		//	if (k < _this->key) {
		//		if (isBlack(_this->left) and _this->left
		//				and isBlack(_this->left->left))
		//			_this = &_this->moveRedLeft();
		//		_this->left = _remove(_this->left, k);
		//	} else {
		//		if (isRed(_this->left))
		//			_this = &rotateRight(_this);
		//
		//		if (k == _this->key and !_this->right) {
		//			delete _this;
		//			return nullptr;
		//		}
		//
		//		if (isBlack(_this->right) and _this->right
		//				and isBlack(_this->right->left))
		//			_this = &_this->moveRedRight();
		//
		//		if (k == _this->key) {
		//			_this->key = _this->right->min().key;
		//			_this->right = _this->right->_delete_min();
		//		} else {
		//			if (_this->right == nullptr) {
		//				throw std::runtime_error(
		//						"item is not present in the tree and delete alg. may destroyed the tree");
		//			}
		//			_this->right = _remove(_this->right, k);
		//		}
		//	}
		//	return &_this->fixUp();
		//}
		//
		//Node_t & min() {
		//	if (left)
		//		return left->min();
		//	return *this;
		//}
		//Node_t & max() {
		//	if (right)
		//		return right->max();
		//	return *this;
		//}

	};

protected:
	Node_t *root;
	size_t heightBLACK;      // black height of tree

	static size_t height(const Node_t * x) {
		if (x == nullptr)
			return 0;
		else
			return x->height;
	}

	// Update height and N.
	static Node_t & setN(Node_t & h) {
		h.N = size(h.left) + size(h.right) + 1;

		if (height(h.left) > height(h.right))
			h.height = height(h.left) + 1;
		else
			h.height = height(h.right) + 1;

		return h;
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
	static Node_t & rotateLeft(Node_t & h) {
		auto & x = *h.right;
		h.right = x.left;
		x.left = &setN(h);
		x.color = x.left->color;
		x.left->color = RED;
		return setN(x);
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
	static Node_t & rotateRight(Node_t & h) {
		auto & x = *h.left;
		h.left = x.right;
		x.right = &setN(h);
		x.color = x.right->color;
		x.right->color = RED;
		return setN(x);
	}
	/* Fix the Left-leaning Red-black tree properties
	 * with upto two rotations and a possible color flip.
	 */
	static Node_t & fixUp(Node_t * h) {
		if (isRed(h->right)) {
			h = &rotateLeft(*h);
		}
		if (isRed(h->left) and h->left and isRed(h->left->left)) {
			h = &rotateRight(*h);
		}

		if (isRed(h->left) and isRed(h->right)) {
			h->_flip_colors();
		}
		return setN(*h);
	}

	/*
	 *  Assuming that self is red and both self.left and self.left.left
	 *  are black, make self.left or one of its children red.
	 */
	static Node_t & moveRedLeft(Node_t * h) {
		h->_flip_colors();
		if (isRed(h->right->left)) {
			h->right = &rotateRight(*h->right);
			h = &rotateLeft(*h);
			h->_flip_colors();
		}
		return *h;
	}

	/* Assuming that self is red and both right and right.left
	 * are black, make right or one of its children red.
	 */
	static Node_t & moveRedRight(Node_t * h) {
		h->_flip_colors();
		if (isRed(h->left->left)) {
			h = &rotateRight(*h);
			h->_flip_colors();
		}
		return *h;
	}
	/*
	 **/
	Node_t & insert(Node_t * h, const KEY_t & key) {
		if (h == nullptr)
			return *(new Node_t(key));

		if (species == LLRB_NODE::TD234)
			if (isRed(h->left) and isRed(h->right))
				h->_flip_colors();

		if (eq()(key, h->key))
			throw std::runtime_error("duplicit value");
		else if (less()(key, h->key))
			h->left = &insert(h->left, key);
		else
			h->right = &insert(h->right, key);

		if (species == LLRB_NODE::BST)
			return setN(*h);

		if (isRed(h->right))
			h = &rotateLeft(*h);

		if (isRed(h->left) and isRed(h->left->left))
			h = &rotateRight(*h);

		if (species == LLRB_NODE::BU23)
			if (isRed(h->left) and isRed(h->right))
				h->_flip_colors();

		return setN(*h);
	}
	static size_t size(const Node_t * x) {
		if (x == nullptr)
			return 0;
		else
			return x->N;
	}
	static const KEY_t & min(const Node_t & x) {
		if (x.left == nullptr)
			return x.key;
		else
			return min(*x.left);
	}

	static Node_t * removeMin(Node_t * h) {
		if (h->left == nullptr) {
			delete h;
			return nullptr;
		}

		if (!isRed(h->left) and !isRed(h->left->left))
			h = &moveRedLeft(h);

		h->left = removeMin(h->left);

		return &fixUp(h);
	}

	static Node_t * remove(Node_t * h, const KEY_t key) {
		bool _eq = eq()(key, h->key);
		if (less()(key, h->key) and not _eq) {
			if (!isRed(h->left) and !isRed(h->left->left))
				h = &moveRedLeft(h);
			h->left = remove(h->left, key);
		} else {
			if (isRed(h->left))
				h = &rotateRight(*h);
			if (_eq and (h->right == nullptr)) {
				delete h;
				return nullptr;
			}

			if (!isRed(h->right) and !isRed(h->right->left))
				h = &moveRedRight(h);
			if (eq()(key, h->key)) {
				h->key = min(*h->right);
				h->right = removeMin(h->right);
			} else
				h->right = remove(h->right, key);
		}

		return &fixUp(h);
	}

public:
	LLRBTree() :
			root(nullptr), heightBLACK(0) {
	}

	const constexpr Node_t * get_root() const {
		return root;
	}
	void insert(const std::vector<KEY_t> & keys) {
		for (const auto & k: keys) {
			insert(k);
		}
	}
	void insert(const KEY_t & key) {
		root = &insert(root, key);
		if (isRed(root))
			heightBLACK++;
		root->color = BLACK;
	}

	// Delete a node with the given key from the tree.
	void remove(const KEY_t & k) {
		root = remove(root, k);
		if (root)
			root->color = BLACK;
	}

	///*
	// * Merge second tree to this tree
	// * :note: Items are removed from second tree
	// * */
	////void merge(LLRBTree &);
	//Node_t* find(const KEY_t & key) {
	//	if (root) {
	//		root->find(key);
	//	}
	//	return nullptr;
	//}

	size_t size() const {
		return size(root);
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

	~LLRBTree() {
		while(root)
			root = removeMin(root);
	}
};
