#pragma once

#include <functional>
#include <vector>
#include <stack>
#include <algorithm>
#include "../ElementaryClasses.h"

/* comment out the line below to remove all the debugging assertion */
/* checks from the compiled code.  */
#define DEBUG_ASSERT 0

class rb_red_blk_tree {
public:
	using FieldOrder_t = const std::vector<int>&;
  //Total 29 bytes per node
  struct rb_red_blk_node {
  	Range1d key;
  	rb_red_blk_node* left;
  	rb_red_blk_node* right;
  	rb_red_blk_tree* rb_tree_next_level;
  	//int priority; /*max_priority of all children*/
  	bool red; /* if red=0 then the node is black */
  	rb_red_blk_node* parent;
  };

  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  rb_red_blk_node* root;
  rb_red_blk_node* nil;

  int count = 0;
  std::vector<Range1d> chain_boxes;
  //std::priority_queue<int> pq;
  std::vector<int> priority_list;
  int max_priority_local;

  // FOR RB tree light weight node
  rb_red_blk_tree();

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
  std::stack<void *> * RBEnumerate(const Range1d& low, const Range1d& high);

  void PushPriority(int p) {
	  max_priority_local = std::max(p, max_priority_local);
	  priority_list.push_back(p);
  }
  void PopPriority(int p) {
	  find(begin(priority_list), end(priority_list),p);
	  priority_list.erase(find(begin(priority_list), end(priority_list), p));
	  if (p == max_priority_local ) {
		  max_priority_local = *std::max_element(begin(priority_list), end(priority_list));
	  }
	  if (priority_list.empty()) max_priority_local = -1;
  }
  void ClearPriority() {
	  max_priority_local = -1;
	  priority_list.clear();
  }
  int GetMaxPriority() const {
	  return max_priority_local;
  }
  int GetSizeList() const{
	  return priority_list.size();
  }
  /***********************************************************************/
  /*  FUNCTION:  RBDelete */
  /**/
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
  void RBDelete(rb_red_blk_node* );

  std::vector<std::pair<rb_red_blk_tree*, rb_red_blk_node *>> RBFindNodeSequence(
		  const std::vector<Range1d>& key, int level,
		  const std::vector<int>& fieldOrder);

  rb_red_blk_node * RBTreeInsertWithPathCompression(
		  const std::vector<Range1d>& key, size_t level, FieldOrder_t fieldOrder, int priority);

  bool TreeInsertWithPathCompressionHelp(
		  rb_red_blk_node* z, const std::vector<Range1d>& b, size_t level,
		  FieldOrder_t fieldOrder, int priority, rb_red_blk_node*& out_ptr);
  int RBExactQueryPriority(const Packet& q, size_t level, FieldOrder_t fieldOrder, int priority_so_far);
  bool TreeInsertHelp(rb_red_blk_node* z, const std::vector<Range1d>& b, size_t level,
		  FieldOrder_t fieldOrder, int priority,  rb_red_blk_node*& out_ptr);

  /*  Before calling Insert RBTree the node x should have its key set */
  /***********************************************************************/
  /*  FUNCTION:  RBTreeInsert */
  /**/
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
  rb_red_blk_node * RBTreeInsert(const std::vector<Range1d>& key, size_t level,
		  const std::vector<int>& fieldOrder, int priority=0);

  bool RBTreeCanInsert(const std::vector<Range1d>& z, size_t level, FieldOrder_t fieldOrder);
  std::vector<Rule> RBSerializeIntoRules(FieldOrder_t fieldOrder);

  /***********************************************************************/
  /*  FUNCTION:  RBExactQuery */
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
  int RBExactQuery(const Packet& q, size_t level, FieldOrder_t fieldOrder);

  void RBSerializeIntoRulesRecursion(rb_red_blk_node* node, size_t level,
		  FieldOrder_t fieldOrder, std::vector<Range1d>& boxes_so_far, std::vector<Rule>& rules_so_far);

  int RBExactQueryIterative(const Packet& q, FieldOrder_t fieldOrder, size_t level=0);

  int CalculateMemoryConsumptionRecursion(rb_red_blk_node * node, size_t level, FieldOrder_t fieldOrder);
  int CalculateMemoryConsumption(FieldOrder_t fieldOrder);

  /***********************************************************************/
  /*  FUNCTION:  RBTreePrint */
  /**/
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
  void RBTreePrint();
  void RBTreeDestroy();

  /***********************************************************************/
  /*  FUNCTION:  Treepredecessor  */
  /**/
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
  rb_red_blk_node* TreePredecessor(rb_red_blk_node*);

  /***********************************************************************/
  /*  FUNCTION:  TreeSuccessor  */
  /**/
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
  rb_red_blk_node* TreeSuccessor(rb_red_blk_node*);

  static void RBTreeDeleteWithPathCompression(rb_red_blk_tree*& tree,
		  const std::vector<Range1d>& key, size_t level,
		  FieldOrder_t fieldOrder, int priority, bool& JustDeletedTree);

  /***********************************************************************/
  /*  FUNCTION:  RBTreeDestroy */
  /**/
  /*    INPUTS:  tree is the tree to destroy */
  /**/
  /*    OUTPUT:  none */
  /**/
  /*    EFFECT:  Destroys the key and frees memory */
  /**/
  /*    Modifies Input: tree */
  /**/
  /***********************************************************************/
  ~rb_red_blk_tree();
private:
  /***********************************************************************/
  /*  FUNCTION:  RighttRotate */
  /**/
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
  void RightRotate(rb_red_blk_node* y);

  /***********************************************************************/
  /*  FUNCTION:  RBDeleteFixUp */
  /**/
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
  void RBDeleteFixUp(rb_red_blk_node* x);

  /***********************************************************************/
  /*  FUNCTION:  LeftRotate */
  /**/
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
  void LeftRotate(rb_red_blk_node* x);

  /***********************************************************************/
  /*  FUNCTION:  InorderTreePrint */
  /**/
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
  void InorderTreePrint(rb_red_blk_node* x);

  /***********************************************************************/
  /*  FUNCTION:  TreeDestHelper */
  /**/
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
  void TreeDestHelper(rb_red_blk_node* x);
};

bool inline Overlap(unsigned int a1, unsigned  int a2, unsigned int b1, unsigned int b2) {
	if (a1 <= b1) {
		return b1 <= a2;
	} else {
		return a1 <= b2;
	}
}





void NullFunction(void*);

