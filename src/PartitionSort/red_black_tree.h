#pragma once

#include <functional>
#include <vector>
#include <stack>
#include <algorithm>
#include "../ElementaryClasses.h"

// comment out the line below to remove all the debugging assertion
// checks from the compiled code.
#define DEBUG_ASSERT 0

class RedBlackTree {
public:
	using FieldOrder_t = const std::vector<int>&;
	//Total 29 bytes per node
	class RedBlackTree_node {
	public:
		Range1d key;
		RedBlackTree_node* left;
		RedBlackTree_node* right;
		/*
		 * Created if inserted node equals to this node
		 * */
		RedBlackTree* rb_tree_next_level;
		//int priority; /*max_priority of all children*/
		bool red; /* if red=0 then the node is black */
		RedBlackTree_node* parent;
	};

	/*  A sentinel is used for root and for nil.  These sentinels are */
	/*  created when RBTreeCreate is caled.  root->left should always */
	/*  point to the node which is the root of the tree.  nil points to a */
	/*  node which should always be black but has aribtrary children and */
	/*  parent and no key or info.  The point of using these sentinels is so */
	/*  that the root and nil nodes do not require special cases in the code */
	RedBlackTree_node* root;

	int count = 0;
	std::vector<Range1d> chain_boxes;
	//std::priority_queue<int> pq;
	std::vector<int> priority_list;
	int max_priority_local;

	// FOR RB tree light weight node
	RedBlackTree();

	/***********************************************************************/
	/*  FUNCTION:  RBDEnumerate */
	/**/
	/*    INPUTS:  tree is the tree to look for keys >= low */
	/*             and <= high with respect to the Compare function */
	/**/
	/*    OUTPUT:  stack containing pointers to the nodes between [low,high] */
	/**/
	/*    Modifies Input: none */
	/***********************************************************************/
	std::stack<RedBlackTree_node*> * RBEnumerate(const Range1d& low, const Range1d& high);

	void pushPriority(int p);
	void popPriority(int p);
	void clearPriority();
	int getMaxPriority() const;
	int getSizeList() const;

	/***********************************************************************/
	/*    INPUTS:  tree is the tree to delete node z from */
	/**/
	/*    OUTPUT:  none */
	/**/
	/*    EFFECT:  Deletes z from tree and frees the key and info of z */
	/*             using DestoryKey and DestoryInfo.  Then calls */
	/*             RBDeleteFixUp to restore red-black properties */
	/**/
	/*    Modifies Input: tree, z */
	/**/
	/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
	/***********************************************************************/
	void deleteNode(RedBlackTree_node*);

	std::vector<std::pair<RedBlackTree*, RedBlackTree_node *>> findNodeSequence(
			const std::vector<Range1d>& key, int level,
			const std::vector<int>& fieldOrder);

	/*  Before calling Insert RBTree the node x should have its key set */
	/***********************************************************************/
	/*  INPUTS:  tree is the red-black tree to insert a node which has a key */
	/*           pointed to by key and info pointed to by info.  */
	/**/
	/*  OUTPUT:  This function returns a pointer to the newly inserted node */
	/*           which is guarunteed to be valid until this node is deleted. */
	/*           What this means is if another data structure stores this */
	/*           pointer then the tree does not need to be searched when this */
	/*           is to be deleted. */
	/**/
	/*  Modifies Input: tree */
	/**/
	/*  EFFECTS:  Creates a node node which contains the appropriate key and */
	/*            info pointers and inserts it into the tree. */
	/***********************************************************************/
	RedBlackTree_node * insertWithPathCompression(
			const std::vector<Range1d>& key, size_t level,
			FieldOrder_t fieldOrder, int priority);

	/***********************************************************************/
	/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
	/**/
	/*  OUTPUT:  none */
	/**/
	/*  Modifies Input:  tree, z */
	/**/
	/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
	/*            using the algorithm described in _Introduction_To_Algorithms_ */
	/*            by Cormen et al.  This funciton is only intended to be called */
	/*            by the RBTreeInsert function and not by the user */
	/***********************************************************************/
	bool insertWithPathCompressionHelp(RedBlackTree_node* z,
			const std::vector<Range1d>& b, size_t level,
			FieldOrder_t fieldOrder, int priority, RedBlackTree_node*& out_ptr);
	int exactQueryPriority(const Packet& q, size_t level,
			FieldOrder_t fieldOrder, int priority_so_far);

	/*
	 * @return true if value specified by b exists in tree
	 * */
	bool insert_and_check_if_exists(RedBlackTree_node* z, const std::vector<Range1d>& b,
			size_t level, FieldOrder_t fieldOrder, int priority,
			RedBlackTree_node*& out_ptr);

	/*  Before calling Insert RBTree the node x should have its key set */
	/***********************************************************************/
	/*  INPUTS:  tree is the red-black tree to insert a node which has a key */
	/*           pointed to by key and info pointed to by info.  */
	/**/
	/*  OUTPUT:  This function returns a pointer to the newly inserted node */
	/*           which is guarunteed to be valid until this node is deleted. */
	/*           What this means is if another data structure stores this */
	/*           pointer then the tree does not need to be searched when this */
	/*           is to be deleted. */
	/**/
	/*  Modifies Input: tree */
	/**/
	/*  EFFECTS:  Creates a node node which contains the appropriate key and */
	/*            info pointers and inserts it into the tree. */
	/***********************************************************************/
	RedBlackTree_node * insert(const std::vector<Range1d>& key, size_t level,
			const std::vector<int>& fieldOrder, int priority = 0);

	/*
	 * Regular red-black tree insertFix method
	 * */
	void _insertFix(RedBlackTree_node * x);

	bool canInsert(const std::vector<Range1d>& z, size_t level,
			FieldOrder_t fieldOrder);
	std::vector<Rule> serializeIntoRules(FieldOrder_t fieldOrder);

	/**********************************************************************
	 *    INPUTS:  tree: 'tree' at 'level' for current 'fieldOrder'
	 *
	 *    OUTPUT:  a vector of Rule
	 *
	 *    Modifies Input: none
	 ***********************************************************************/
	void serializeIntoRulesRecursion(RedBlackTree_node* node, size_t level,
			FieldOrder_t fieldOrder, std::vector<Range1d>& boxes_so_far,
			std::vector<Rule>& rules_so_far);

	/***********************************************************************/
	/**/
	/*    INPUTS:  tree is the tree to print and q is a pointer to the key */
	/*             we are searching for */
	/**/
	/*    OUTPUT:  returns the a node with key equal to q.  If there are */
	/*             multiple nodes with key equal to q this function returns */
	/*             the one highest in the tree */
	/**/
	/*    Modifies Input: none */
	/**/
	/***********************************************************************/
	int exactQuery(const Packet& q, size_t level, FieldOrder_t fieldOrder);

	int exactQueryIterative(const Packet& q, FieldOrder_t fieldOrder,
			size_t level = 0);

	int calculateMemoryConsumptionRecursion(RedBlackTree_node * node,
			size_t level, FieldOrder_t fieldOrder);
	int calculateMemoryConsumption(FieldOrder_t fieldOrder);

	/***********************************************************************/
	/*    INPUTS:  tree is the tree to print */
	/**/
	/*    OUTPUT:  none */
	/**/
	/*    EFFECT:  This function recursively prints the nodes of the tree */
	/*             inorder using the PrintKey and PrintInfo functions. */
	/**/
	/*    Modifies Input: none */
	/**/
	/***********************************************************************/
	void print();
	void destroy();

	/***********************************************************************/
	/*    INPUTS:  tree is the tree in question, and x is the node we want the */
	/*             the predecessor of. */
	/**/
	/*    OUTPUT:  This function returns the predecessor of x or NULL if no */
	/*             predecessor exists. */
	/**/
	/*    Modifies Input: none */
	/**/
	/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
	/***********************************************************************/
	RedBlackTree_node* getPredecessor(RedBlackTree_node*);

	/***********************************************************************/
	/*    INPUTS:  tree is the tree in question, and x is the node we want the */
	/*             the successor of. */
	/**/
	/*    OUTPUT:  This function returns the successor of x or NULL if no */
	/*             successor exists. */
	/**/
	/*    Modifies Input: none */
	/**/
	/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
	/***********************************************************************/
	RedBlackTree_node* getSuccessor(RedBlackTree_node*);

	static void deleteWithPathCompression(RedBlackTree*& tree,
			const std::vector<Range1d>& key, size_t level,
			FieldOrder_t fieldOrder, int priority, bool& JustDeletedTree);

	/***********************************************************************/
	/*    EFFECT:  Destroys the key and frees memory */
	/***********************************************************************/
	~RedBlackTree();
private:
	/***********************************************************************/
	/*  INPUTS:  This takes a tree so that it can access the appropriate */
	/*           root and nil pointers, and the node to rotate on. */
	/**/
	/*  OUTPUT:  None */
	/**/
	/*  Modifies Input?: tree, y */
	/**/
	/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
	/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
	/*            makes the parent of x be to the left of x, x the parent of */
	/*            its parent before the rotation and fixes other pointers */
	/*            accordingly. */
	/***********************************************************************/
	void rotateRight(RedBlackTree_node* y);

	/***********************************************************************/
	/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
	/*             out node in RBTreeDelete. */
	/**/
	/*    OUTPUT:  none */
	/**/
	/*    EFFECT:  Performs rotations and changes colors to restore red-black */
	/*             properties after a node is deleted */
	/**/
	/*    Modifies Input: tree, x */
	/**/
	/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
	/***********************************************************************/
	void deleteFixUp(RedBlackTree_node* x);

	/***********************************************************************/
	/*  INPUTS:  This takes a tree so that it can access the appropriate */
	/*           root and nil pointers, and the node to rotate on. */
	/**/
	/*  OUTPUT:  None */
	/**/
	/*  Modifies Input: tree, x */
	/**/
	/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
	/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
	/*            makes the parent of x be to the left of x, x the parent of */
	/*            its parent before the rotation and fixes other pointers */
	/*            accordingly. */
	/***********************************************************************/
	void rotateLeft(RedBlackTree_node* x);

	/***********************************************************************/
	/*    INPUTS:  tree is the tree to print and x is the current inorder node */
	/**/
	/*    OUTPUT:  none  */
	/**/
	/*    EFFECTS:  This function recursively prints the nodes of the tree */
	/*              inorder using the PrintKey and PrintInfo functions. */
	/**/
	/*    Modifies Input: none */
	/**/
	/*    Note:    This function should only be called from RBTreePrint */
	/***********************************************************************/
	void printInorder(RedBlackTree_node* x);

	/***********************************************************************/
	/*    INPUTS:  tree is the tree to destroy and x is the current node */
	/**/
	/*    OUTPUT:  none  */
	/**/
	/*    EFFECTS:  This function recursively destroys the nodes of the tree */
	/*              postorder using the DestroyKey and DestroyInfo functions. */
	/**/
	/*    Modifies Input: tree, x */
	/**/
	/*    Note:    This function should only be called by RBTreeDestroy */
	/***********************************************************************/
	void destHelper(RedBlackTree_node* x);
};
