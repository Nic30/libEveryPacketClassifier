// based on https://github.com/anandarao/Red-Black-Tree/blob/master/RBTree.cpp
//#pragma once
////#include "RBTree.h"
//#include <vector>
//
//template<typename KEY_t>
//RBNode<KEY_t>::RBNode(KEY_t key) {
//	left = right = parent = nullptr;
//	this->key = key;
//	color = RED;
//}
//
//template<typename KEY_t>
//RBTree<KEY_t>::RBTree() {
//	root = nullptr;
//	_M_size = 0;
//}
//
//template<typename KEY_t>
//RBTree<KEY_t>::RBTree(std::vector<KEY_t> & values) : RBTree<KEY_t>() {
//	for(auto & v: values) {
//		add(v);
//	}
//}
//
//template<typename KEY_t>
//int RBTree<KEY_t>::getColor(Node_t *&node) {
//	if (node == nullptr)
//		return BLACK;
//
//	return node->color;
//}
//
//template<typename KEY_t>
//void RBTree<KEY_t>::setColor(Node_t *&node, Color color) {
//	if (node == nullptr)
//		return;
//
//	node->color = color;
//}
//
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::addBST(Node_t *&root,
//		const KEY_t & new_key, Node_t *& new_node, Node_t *& existing_node) {
//
//	if (root == nullptr) {
//		new_node = new Node_t(new_key);
//		return new_node;
//	}
//
//	if (new_key == root->key) {
//		existing_node = root;
//	} else if (new_key < root->key) {
//		root->left = addBST(root->left, new_key, new_node, existing_node);
//		root->left->parent = root;
//	} else { //if (ptr->key > root->key) {
//		root->right = addBST(root->right, new_key, new_node, existing_node);
//		root->right->parent = root;
//	}
//
//	return root;
//}
//
//template<typename KEY_t>
//bool RBTree<KEY_t>::add(const KEY_t & k) {
//	Node_t * new_node = nullptr;
//	Node_t * existing_node = nullptr;
//	root = addBST(root, k, new_node, existing_node);
//	if (new_node) {
//		fixAddRBTree(new_node);
//		_M_size++;
//		return true;
//	}
//	return false;
//}
//
//template<typename KEY_t>
//void RBTree<KEY_t>::rotateLeft(Node_t *&ptr) {
//	//
//	//          |                                  |
//	//          X                                  Y
//	//         / \         left rotate            / \
//	//        A  Y       ------------->          X   γ
//	//           / \                            / \
//	//          B  γ                           A  B
//	//
//	// It should be note that during the rotating we do not change
//	// the Nodes' color.
//	//
//	Node_t *right_child = ptr->right;
//	ptr->right = right_child->left;
//
//	if (ptr->right != nullptr)
//		ptr->right->parent = ptr;
//
//	right_child->parent = ptr->parent;
//
//	if (ptr->parent == nullptr)
//		root = right_child;
//	else if (ptr == ptr->parent->left)
//		ptr->parent->left = right_child;
//	else
//		ptr->parent->right = right_child;
//
//	right_child->left = ptr;
//	ptr->parent = right_child;
//}
//
//template<typename KEY_t>
//void RBTree<KEY_t>::rotateRight(Node_t *&ptr) {
//	//
//	//          |                                  |
//	//          X                                  Y
//	//         / \         right rotate           / \
//	//        Y   γ      ------------->          A   X
//	//       / \                                    / \
//	//      A  B                                 B  γ
//	//
//	// It should be note that during the rotating we do not change
//	// the Nodes' color.
//	//
//	Node_t *left_child = ptr->left;
//	ptr->left = left_child->right;
//
//	if (ptr->left != nullptr)
//		ptr->left->parent = ptr;
//
//	left_child->parent = ptr->parent;
//
//	if (ptr->parent == nullptr)
//		root = left_child;
//	else if (ptr == ptr->parent->left)
//		ptr->parent->left = left_child;
//	else
//		ptr->parent->right = left_child;
//
//	left_child->right = ptr;
//	ptr->parent = left_child;
//}
//
//template<typename KEY_t>
//void RBTree<KEY_t>::fixAddRBTree(Node_t *&ptr) {
//	Node_t *parent = nullptr;
//	Node_t *grandparent = nullptr;
//	while (ptr != root && getColor(ptr) == RED && getColor(ptr->parent) == RED) {
//		parent = ptr->parent;
//		grandparent = parent->parent;
//		if (parent == grandparent->left) {
//			Node_t *uncle = grandparent->right;
//			if (getColor(uncle) == RED) {
//				setColor(uncle, BLACK);
//				setColor(parent, BLACK);
//				setColor(grandparent, RED);
//				ptr = grandparent;
//			} else {
//				if (ptr == parent->right) {
//					rotateLeft(parent);
//					ptr = parent;
//					parent = ptr->parent;
//				}
//				rotateRight(grandparent);
//				std::swap(parent->color, grandparent->color);
//				ptr = parent;
//			}
//		} else {
//			Node_t *uncle = grandparent->left;
//			if (getColor(uncle) == RED) {
//				setColor(uncle, BLACK);
//				setColor(parent, BLACK);
//				setColor(grandparent, RED);
//				ptr = grandparent;
//			} else {
//				if (ptr == parent->left) {
//					rotateRight(parent);
//					ptr = parent;
//					parent = ptr->parent;
//				}
//				rotateLeft(grandparent);
//				std::swap(parent->color, grandparent->color);
//				ptr = parent;
//			}
//		}
//	}
//	setColor(root, BLACK);
//}
//
//template<typename KEY_t>
//void RBTree<KEY_t>::fixDiscardRBTree(Node_t *&node) {
//	if (node == nullptr)
//		return;
//
//	if (node == root) {
//		Node_t * _n = node;
//		root = nullptr;
//		if (_n->left) {
//			root = _n->left;
//			setColor(root, BLACK);
//			assert(_n->right == nullptr);
//		}
//		if (_n->right) {
//			root = _n->right;
//			setColor(root, BLACK);
//		}
//		delete _n;
//		return;
//	}
//
//	if (getColor(node) == RED || getColor(node->left) == RED
//			|| getColor(node->right) == RED) {
//		Node_t *child = node->left != nullptr ? node->left : node->right;
//
//		if (node == node->parent->left) {
//			node->parent->left = child;
//			if (child != nullptr)
//				child->parent = node->parent;
//			setColor(child, BLACK);
//			delete (node);
//		} else {
//			node->parent->right = child;
//			if (child != nullptr)
//				child->parent = node->parent;
//			setColor(child, BLACK);
//			delete (node);
//		}
//	} else {
//		Node_t *sibling = nullptr;
//		Node_t *parent = nullptr;
//		Node_t *ptr = node;
//		setColor(ptr, DOUBLE_BLACK);
//		while (ptr != root && getColor(ptr) == DOUBLE_BLACK) {
//			parent = ptr->parent;
//			if (ptr == parent->left) {
//				sibling = parent->right;
//				if (getColor(sibling) == RED) {
//					setColor(sibling, BLACK);
//					setColor(parent, RED);
//					rotateLeft(parent);
//				} else {
//					if (getColor(sibling->left) == BLACK
//							&& getColor(sibling->right) == BLACK) {
//						setColor(sibling, RED);
//						if (getColor(parent) == RED)
//							setColor(parent, BLACK);
//						else
//							setColor(parent, DOUBLE_BLACK);
//						ptr = parent;
//					} else {
//						if (getColor(sibling->right) == BLACK) {
//							setColor(sibling->left, BLACK);
//							setColor(sibling, RED);
//							rotateRight(sibling);
//							sibling = parent->right;
//						}
//						setColor(sibling, parent->color);
//						setColor(parent, BLACK);
//						setColor(sibling->right, BLACK);
//						rotateLeft(parent);
//						break;
//					}
//				}
//			} else {
//				sibling = parent->left;
//				if (getColor(sibling) == RED) {
//					setColor(sibling, BLACK);
//					setColor(parent, RED);
//					rotateRight(parent);
//				} else {
//					if (getColor(sibling->left) == BLACK
//							&& getColor(sibling->right) == BLACK) {
//						setColor(sibling, RED);
//						if (getColor(parent) == RED)
//							setColor(parent, BLACK);
//						else
//							setColor(parent, DOUBLE_BLACK);
//						ptr = parent;
//					} else {
//						if (getColor(sibling->left) == BLACK) {
//							setColor(sibling->right, BLACK);
//							setColor(sibling, RED);
//							rotateLeft(sibling);
//							sibling = parent->left;
//						}
//						setColor(sibling, parent->color);
//						setColor(parent, BLACK);
//						setColor(sibling->left, BLACK);
//						rotateRight(parent);
//						break;
//					}
//				}
//			}
//		}
//		if (node == node->parent->left)
//			node->parent->left = nullptr;
//		else
//			node->parent->right = nullptr;
//		delete (node);
//		setColor(root, BLACK);
//	}
//}
//
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::discardBST(Node_t *&root,
//		KEY_t data) {
//	if (root == nullptr)
//		return root;
//
//	if (data < root->key)
//		return discardBST(root->left, data);
//
//	if (data > root->key)
//		return discardBST(root->right, data);
//
//	if (root->left == nullptr || root->right == nullptr) {
//		if (root->key == data)
//			return root;
//		else
//			return nullptr;
//	}
//
//	Node_t *temp = minValueNode(root->right);
//	root->key = temp->key;
//	return discardBST(root->right, temp->key);
//}
//
//template<typename KEY_t>
//bool RBTree<KEY_t>::discard(const KEY_t & data) {
//	Node_t *node = discardBST(root, data);
//	fixDiscardRBTree(node);
//	if (node) {
//		_M_size--;
//		return true;
//	}
//	return false;
//}
//
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::minValueNode(Node_t *&node) {
//	Node_t *ptr = node;
//
//	while (ptr->left != nullptr)
//		ptr = ptr->left;
//
//	return ptr;
//}
//
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::maxValueNode(Node_t *&node) {
//	Node_t *ptr = node;
//
//	while (ptr->right != nullptr)
//		ptr = ptr->right;
//
//	return ptr;
//}
//
//template<typename KEY_t>
//int RBTree<KEY_t>::getBlackHeight(Node_t *node) {
//	int blackheight = 0;
//	while (node != nullptr) {
//		if (getColor(node) == BLACK)
//			blackheight++;
//		node = node->left;
//	}
//	return blackheight;
//}
//
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::search(KEY_t & key) {
//	if (root == nullptr) {
//		return nullptr;
//	}
//	Node_t * tmp = root;
//	while (tmp) {
//		if (tmp->key == key)
//			return tmp;
//		else {
//			if (tmp->key < key)
//				tmp = tmp->right;
//			else
//				tmp = tmp->left;
//		}
//	}
//	return nullptr;
//}
//
///*
// * :note: the root of returned tree may violates the color constraint
// */
//template<typename KEY_t>
//typename RBTree<KEY_t>::Node_t* RBTree<KEY_t>::merge_relax(Node_t *a, Node_t *b) {
//    if (rank(a) < rank(b)) {
//        assert (not b->is_leaf);
//        return set_left(b, merge_relax(a, b->left));
//    } else if (rank(a) > rank(b)) {
//        assert (not a->is_leaf);
//        return set_right(a, merge_relax(a->right, b));
//    } else {
//        a->m_color = BLACK;
//        b->m_color = BLACK;
//        return new Node_t(a, b, RED);
//    }
//}
//
////https://github.com/kmyk/competitive-programming-library/blob/387463716c082d2e7d5a49111b41fd95e76e9032/data-structure/red-black-tree.inc.cpp#L59
//template<typename KEY_t>
//void RBTree<KEY_t>::merge(RBTree & rbTree2) {
//        if (root == nullptr) {
//        	root = rbTree2.root;
//        	_M_size = rbTree2._M_size;
//        	return;
//        }
//        if (rbTree2.root == nullptr)
//        	return;
//
//        root = merge_relax(a, b);
//        root->m_color = BLACK;
//}
//
////	KEY_t temp;
////	Node_t *c, *temp_ptr;
////	Node_t *root1 = root;
////	Node_t *root2 = rbTree2.root;
////	int initialblackheight1 = getBlackHeight(root1);
////	int initialblackheight2 = getBlackHeight(root2);
////	if (initialblackheight1 > initialblackheight2) {
////		c = maxValueNode(root1);
////		temp = c->key;
////		discard(c->key);
////		root1 = root;
////	} else if (initialblackheight2 > initialblackheight1) {
////		c = minValueNode(root2);
////		temp = c->key;
////		rbTree2.discard(c->key);
////		root2 = rbTree2.root;
////	} else {
////		c = minValueNode(root2);
////		temp = c->key;
////		rbTree2.discard(c->key);
////		root2 = rbTree2.root;
////		if (initialblackheight1 != getBlackHeight(root2)) {
////			rbTree2.add(c->key);
////			root2 = rbTree2.root;
////			c = maxValueNode(root1);
////			temp = c->key;
////			discard(c->key);
////			root1 = root;
////		}
////	}
////	setColor(c, RED);
////	int finalblackheight1 = getBlackHeight(root1);
////	int finalblackheight2 = getBlackHeight(root2);
////	if (finalblackheight1 == finalblackheight2) {
////		c->left = root1;
////		root1->parent = c;
////		c->right = root2;
////		root2->parent = c;
////		setColor(c, BLACK);
////		c->key = temp;
////		root = c;
////	} else if (finalblackheight2 > finalblackheight1) {
////		Node_t *ptr = root2;
////		while (finalblackheight1 != getBlackHeight(ptr)) {
////			temp_ptr = ptr;
////			ptr = ptr->left;
////		}
////		Node_t *ptr_parent;
////		if (ptr == nullptr)
////			ptr_parent = temp_ptr;
////		else
////			ptr_parent = ptr->parent;
////		c->left = root1;
////		if (root1 != nullptr)
////			root1->parent = c;
////		c->right = ptr;
////		if (ptr != nullptr)
////			ptr->parent = c;
////		ptr_parent->left = c;
////		c->parent = ptr_parent;
////		if (getColor(ptr_parent) == RED) {
////			fixAddRBTree(c);
////		} else if (getColor(ptr) == RED) {
////			fixAddRBTree(ptr);
////		}
////		c->key = temp;
////		root = root2;
////	} else {
////		Node_t *ptr = root1;
////		while (finalblackheight2 != getBlackHeight(ptr)) {
////			ptr = ptr->right;
////		}
////		Node_t *ptr_parent = ptr->parent;
////		c->right = root2;
////		root2->parent = c;
////		c->left = ptr;
////		ptr->parent = c;
////		ptr_parent->right = c;
////		c->parent = ptr_parent;
////		if (getColor(ptr_parent) == RED) {
////			fixAddRBTree(c);
////		} else if (getColor(ptr) == RED) {
////			fixAddRBTree(ptr);
////		}
////		c->key = temp;
////		root = root1;
////	}
////	return;
////}
//
//template<typename KEY_t>
//size_t RBTree<KEY_t>::size() {
//	return _M_size;
//}
