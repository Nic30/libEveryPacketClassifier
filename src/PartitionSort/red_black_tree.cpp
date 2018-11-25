#include "red_black_tree.h"
#include <assert.h>

int CompareBox(const Range1d& a, const Range1d& b) {
	//size_t compare_size = a.size();
	if (a.low == b.low && a.high == b.high)
		return 0;
	if (Overlap(a.low, a.high, b.low, b.high))
		return 2;
	if (a.high < b.low) {
		return -1;
	} else if (a.low > b.high) {
		return 1;
	}

	return 0;
}

int inline CompareQuery(const Range1d& a, const Packet& q, int level,
		rb_red_blk_tree::FieldOrder_t fieldOrder) {
	if (a.high < q[fieldOrder[level]]) {
		return -1;
	} else if (a.low > q[fieldOrder[level]]) {
		return 1;
	}

	return 0;
}

/***********************************************************************/
/*  INPUTS:  All the inputs are names of functions.  CompFunc takes to */
/*  void pointers to keys and returns 1 if the first argument is */
/*  "greater than" the second.   DestFunc takes a pointer to a key and */
/*  destroys it in the appropriate manner when the node containing that */
/*  key is deleted.  InfoDestFunc is similar to DestFunc except it */
/*  receives a pointer to the info of a node and destroys it. */
/*  PrintFunc receives a pointer to the key of a node and prints it. */
/*  PrintInfo receives a pointer to the info of a node and prints it. */
/*  If RBTreePrint is never called the print functions don't have to be */
/*  defined and NullFunction can be used.  */
/***********************************************************************/

rb_red_blk_tree::rb_red_blk_tree() {
	max_priority_local = -1;
	rb_red_blk_node* temp;
	/*  see the comment in the rb_red_blk_tree structure in red_black_tree.h */
	/*  for information on nil and root */
	temp = this->nil = new rb_red_blk_node;
	temp->parent = temp->left = temp->right = temp;
	temp->red = 0;
	temp->key = {1111, 1111};
	temp = this->root = new rb_red_blk_node;
	temp->parent = temp->left = temp->right = this->nil;
	temp->key = {2222, 2222};
	temp->red = 0;
}

void rb_red_blk_tree::LeftRotate(rb_red_blk_node* x) {
	rb_red_blk_node* y;

	/*  I originally wrote this function to use the sentinel for */
	/*  nil to avoid checking for nil.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of nil.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the nil sentinel */
	/*  and expects the nil sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of nil to be */
	/*  unchanged. */

	y = x->right;
	x->right = y->left;

	if (y->left != nil)
		y->left->parent = x; /* used to use sentinel here */
	/* and do an unconditional assignment instead of testing for nil */

	y->parent = x->parent;

	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;

#ifdef DEBUG_ASSERT
	assert(!this->nil->red && "nil not red in LeftRotate");
#endif
}

void rb_red_blk_tree::RightRotate(rb_red_blk_node* y) {
	rb_red_blk_node* x;

	/*  I originally wrote this function to use the sentinel for */
	/*  nil to avoid checking for nil.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of nil.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the nil sentinel */
	/*  and expects the nil sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of nil to be */
	/*  unchanged. */

	x = y->left;
	y->left = x->right;

	if (nil != x->right)
		x->right->parent = y; /*used to use sentinel here */
	/* and do an unconditional assignment instead of testing for nil */

	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	x->parent = y->parent;
	if (y == y->parent->left) {
		y->parent->left = x;
	} else {
		y->parent->right = x;
	}
	x->right = y;
	y->parent = x;

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not red in RightRotate");
#endif
}
bool inline IsIntersect(unsigned a1, unsigned b1, unsigned a2, unsigned b2) {
	return std::max(a1, a2) <= std::min(b1, b2);
}
bool inline IsIdentical(unsigned a1, unsigned b1, unsigned a2, unsigned b2) {
	return a1 == a2 && b1 == b2;
}

bool rb_red_blk_tree::RBTreeCanInsert(const std::vector<Range1d>& z,
		size_t level, FieldOrder_t fieldOrder) {

	if (level == fieldOrder.size()) {
		return true;
	} else if (count == 1) {
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (IsIdentical(z[fieldOrder[i]].low, z[fieldOrder[i]].high,
					chain_boxes[i - level].low, chain_boxes[i - level].high))
				continue;
			if (IsIntersect(z[fieldOrder[i]].low, z[fieldOrder[i]].high,
					chain_boxes[i - level].low, chain_boxes[i - level].high))
				return false;
			else
				return true;
		}
		return true;
	}

	//rb_red_blk_node* y;

	//y = tree->root;
	rb_red_blk_node* x = root->left;
	while (x != nil) {
		//y = x;
		int compare_result = CompareBox(x->key, z[fieldOrder[level]]);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			/*printf("TreeInsertHelp:: Exact Match!\n");
			 return true;
			 x = x->right;*/
			if (level == z.size() - 1 || x->rb_tree_next_level == nullptr)
				return true;
			else
				return x->rb_tree_next_level->RBTreeCanInsert(z, level + 1,
							fieldOrder);
		} else { /* x.key || z.key */
			return false;
		}
	}
	return true;
}

/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
/**/
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

bool rb_red_blk_tree::TreeInsertWithPathCompressionHelp(
		rb_red_blk_node* z, const std::vector<Range1d>& b, size_t level,
		FieldOrder_t fieldOrder, int priority,
		rb_red_blk_node*& out_ptr) {
	/*  This function should only be called by InsertRBTree (see above) */
	rb_red_blk_node* x;
	rb_red_blk_node* y;

	z->left = z->right = nil;
	y = root;
	x = root->left;
	while (x != nil) {
		y = x;
		int compare_result = CompareBox(x->key, z->key);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */

			if (level != fieldOrder.size() - 1) {
				/*if (x->rb_tree_next_level->count == 1) {
				 //path compression

				 auto temp_chain_boxes = x->rb_tree_next_level->chain_boxes;
				 int xpriority = x->rb_tree_next_level->GetMaxPriority();

				 free(x->rb_tree_next_level);
				 //unzipping the next level
				 int run = 1;

				 std::vector<int> naturalFieldOrder(fieldOrder.size());
				 std::iota(begin(naturalFieldOrder), end(naturalFieldOrder), 0);
				 while ( (temp_chain_boxes[run][0] == b[fieldOrder[level + run]][0] && temp_chain_boxes[run][1] == b[fieldOrder[level + run]][1])) {
				 x->rb_tree_next_level = RBTreeCreate();
				 x->rb_tree_next_level->count = 1;
				 x = RBTreeInsert(x->rb_tree_next_level, temp_chain_boxes, level + run, naturalFieldOrder, priority);
				 run++;
				 if (level + run >= fieldOrder.size()) break;
				 }

				 if (level + run  >= fieldOrder.size()) {
				 x->rb_tree_next_level = RBTreeCreate();
				 x->rb_tree_next_level->pq.push(priority);
				 x->rb_tree_next_level->pq.push(xpriority);
				 out_ptr = x;
				 }
				 else if (!(temp_chain_boxes[run][0] == b[fieldOrder[level + run]][0] && temp_chain_boxes[run][1] == b[fieldOrder[level + run]][1])) {
				 //split into z and x node
				 x->rb_tree_next_level = RBTreeCreate();
				 RBTreeInsert(x->rb_tree_next_level, temp_chain_boxes, level + run, naturalFieldOrder, xpriority);
				 RBTreeInsert(x->rb_tree_next_level, b, level + run, fieldOrder, priority);
				 }

				 }
				 else {*/
				x->rb_tree_next_level->RBTreeInsertWithPathCompression(b,
						level + 1, fieldOrder, priority);
				//}

			} else {
				if (x->rb_tree_next_level == nullptr)
					x->rb_tree_next_level = new rb_red_blk_tree();
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->PushPriority(priority);
				out_ptr = x;
			}
			free(z);
			return true;
		} else { /* x.key || z.key */
			printf("Warning TreeInsertPathcompressionHelp : x.key || z.key\n");
		}
	}
	z->parent = y;
	if ((y == root) || (1 == CompareBox(y->key, z->key))) { /* y.key > z.key */
		y->left = z;
	} else {
		y->right = z;
	}
	//found new one to insert to
	//need to create a tree first then followed by insertion
	//we use path-compression here since this tree contains a single rule
	z->rb_tree_next_level = new rb_red_blk_tree();
	z->rb_tree_next_level->RBTreeInsertWithPathCompression(b, level + 1,
			fieldOrder, priority);

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not red in TreeInsertHelp");
#endif
	return false;
}

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

rb_red_blk_tree::rb_red_blk_node * rb_red_blk_tree::RBTreeInsertWithPathCompression(
		const std::vector<Range1d>& key, size_t level,
		FieldOrder_t fieldOrder, int priority) {

	rb_red_blk_node * y;
	rb_red_blk_node * x;
	rb_red_blk_node * newNode;

	if (level == fieldOrder.size()) {
		count++;
		PushPriority(priority);
		return nullptr;
	}

	if (count == 0) {
		//no need to create a node yet; just compress the path
		count++;
		PushPriority(priority);
		//level <= b.size() -1
		for (size_t i = level; i < fieldOrder.size(); i++)
			chain_boxes.push_back(key[fieldOrder[i]]);
		return nullptr;
	}
	if (count == 1) {
		//path compression
		auto temp_chain_boxes = chain_boxes;
		int xpriority = GetMaxPriority();

		//  tree->pq = std::priority_queue<int>();
		count++;
		//  tree->PushPriority(priority);

		/* bool is_identical = 1;
		 for (int i = level; i < fieldOrder.size(); i++) {
		 if (tree->chain_boxes[i - level] != key[fieldOrder[i]]){
		 is_identical = 0;
		 break;
		 }
		 }
		 if (is_identical) {
		 //TODO:: move tree->pq.push(priority); to this line
		 tree->count = 1;
		 return nullptr;
		 }*/
		//quick check if identical just reset count = 1;
		chain_boxes.clear();

		//unzipping the next level

		std::vector<int> naturalFieldOrder(fieldOrder.size());
		std::iota(begin(naturalFieldOrder), end(naturalFieldOrder), 0);
		size_t run = 0;
		if (temp_chain_boxes[run].low == key[fieldOrder[level + run]].low
				&& temp_chain_boxes[run].high
						== key[fieldOrder[level + run]].high) {
			//  printf("[%u %u] vs. [%u %u]\n", temp_chain_boxes[run][0], temp_chain_boxes[run][1], key[fieldOrder[level + run]][0], key[fieldOrder[level + run]][1]);

			x = RBTreeInsert(key, level + run++, fieldOrder, xpriority);
			if (level + run < fieldOrder.size()) {
				while ((temp_chain_boxes[run].low
						== key[fieldOrder[level + run]].low
						&& temp_chain_boxes[run].high
								== key[fieldOrder[level + run]].high)) {

					x->rb_tree_next_level = new rb_red_blk_tree();

					//  x->rb_tree_next_level->PushPriority(xpriority);
					// x->rb_tree_next_level->PushPriority(priority);
					x->rb_tree_next_level->count = 2;
					x = x->rb_tree_next_level->RBTreeInsert(key, level + run,
							fieldOrder, priority);

					run++;

					if (level + run >= fieldOrder.size())
						break;
				}
			}
			if (level + run >= fieldOrder.size()) {
				x->rb_tree_next_level = new rb_red_blk_tree();
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->PushPriority(priority);
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->PushPriority(xpriority);
			} else if (!(temp_chain_boxes[run].low
					== key[fieldOrder[level + run]].low
					&& temp_chain_boxes[run].high
							== key[fieldOrder[level + run]].high)) {
				if (IsIntersect(temp_chain_boxes[run].high,
						temp_chain_boxes[run].high,
						key[fieldOrder[level + run]].low,
						key[fieldOrder[level + run]].high)) {
					printf("Warning not intersect?\n");
					printf("[%u %u] vs. [%u %u]\n",
							temp_chain_boxes[run].low,
							temp_chain_boxes[run].high,
							key[fieldOrder[level + run]].low,
							key[fieldOrder[level + run]].high);
					printf("chain_boxes:\n");
					for (auto e : temp_chain_boxes)
						printf("[%u %u] ", e.low, e.high);
					printf("\n boxes:\n");
					for (size_t i = 0; i < key.size(); i++) {
						printf("[%u %u] ", key[fieldOrder[i]].low,
								key[fieldOrder[i]].high);
					}
					printf("\n");
					exit(0);
				}
				//split into z and x node
				x->rb_tree_next_level = new rb_red_blk_tree();
				//  x->rb_tree_next_level->PushPriority(priority);
				// x->rb_tree_next_level->PushPriority(xpriority);
				auto PrependChainbox =
						[](std::vector<Range1d>& cb, int n_prepend) {
							std::vector<Range1d> t;
							for (int i = 0; i < n_prepend; i++) t.push_back({999, 100020});
							t.insert(end(t), begin(cb), end(cb));
							return t;
						};
				auto z1 = x->rb_tree_next_level->RBTreeInsert(
						PrependChainbox(temp_chain_boxes, level), level + run,
						naturalFieldOrder, xpriority);
				auto z2 = x->rb_tree_next_level->RBTreeInsert(key, level + run,
						fieldOrder, priority);

				x->rb_tree_next_level->count = 2;

				z1->rb_tree_next_level = new rb_red_blk_tree();
				z2->rb_tree_next_level = new rb_red_blk_tree();
				z1->rb_tree_next_level->RBTreeInsertWithPathCompression(
						PrependChainbox(temp_chain_boxes, level),
						level + run + 1, naturalFieldOrder, xpriority);
				z2->rb_tree_next_level->RBTreeInsertWithPathCompression(key,
						level + run + 1, fieldOrder, priority);

			}
		} else {

			auto PrependChainbox =
					[](std::vector<Range1d>& cb, int n_prepend) {
						std::vector<Range1d> t;
						for (int i = 0; i < n_prepend; i++) t.push_back( {0, 10000000});
						t.insert(end(t), begin(cb), end(cb));
						return t;
					};

			auto z1 = RBTreeInsert(
					PrependChainbox(temp_chain_boxes, level), level + run,
					naturalFieldOrder, xpriority);
			auto z2 = RBTreeInsert(key, level + run, fieldOrder,
					priority);
			count = 2;

			z1->rb_tree_next_level = new rb_red_blk_tree();
			z2->rb_tree_next_level = new rb_red_blk_tree();

			z1->rb_tree_next_level->RBTreeInsertWithPathCompression(
					PrependChainbox(temp_chain_boxes, level), level + run + 1,
					naturalFieldOrder, xpriority);
			z2->rb_tree_next_level->RBTreeInsertWithPathCompression(key,
					level + run + 1, fieldOrder, priority);

		}
		return nullptr;
	}

	count++;
	/* tree->PushPriority(priority);
	 int maxpri = tree->GetMaxPriority();
	 tree->priority_list.clear();
	 tree->PushPriority(maxpri);*/

	x = new rb_red_blk_node;
	x->key = key[fieldOrder[level]];
	rb_red_blk_node * out_ptr;

	if (TreeInsertWithPathCompressionHelp(x, key, level, fieldOrder,
			priority, out_ptr)) {
		//insertion finds identical box.
		//do nothing for now
		return out_ptr;
	}

	newNode = x;
	x->red = 1;
	while (x->parent->red) { /* use sentinel instead of checking for root */
		if (x->parent == x->parent->parent->left) {
			y = x->parent->parent->right;
			if (y->red) {
				x->parent->red = 0;
				y->red = 0;
				x->parent->parent->red = 1;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right) {
					x = x->parent;
					LeftRotate(x);
				}
				x->parent->red = 0;
				x->parent->parent->red = 1;
				RightRotate(x->parent->parent);
			}
		} else { /* case for x->parent == x->parent->parent->right */
			y = x->parent->parent->left;
			if (y->red) {
				x->parent->red = 0;
				y->red = 0;
				x->parent->parent->red = 1;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left) {
					x = x->parent;
					RightRotate(x);
				}
				x->parent->red = 0;
				x->parent->parent->red = 1;
				LeftRotate(x->parent->parent);
			}
		}
	}
	root->left->red = 0;

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not red in RBTreeInsert");
	assert(!root->red && "root not red in RBTreeInsert");
#endif

	return (newNode);
}


rb_red_blk_tree::rb_red_blk_node * rb_red_blk_tree::RBTreeInsert(
		const std::vector<Range1d>& key, size_t level,
		FieldOrder_t field_order, int priority) {

	if (level == key.size())
		return nullptr;

	rb_red_blk_node * y;
	rb_red_blk_node * x;
	rb_red_blk_node * newNode;

	x = new rb_red_blk_node;
	x->key = key[field_order[level]];
	rb_red_blk_node * out_ptr;
	if (TreeInsertHelp(x, key, level, field_order, priority, out_ptr)) {
		//insertion finds identical box.
		//do nothing for now
		return out_ptr;
	}

	newNode = x;
	x->red = 1;
	while (x->parent->red) { /* use sentinel instead of checking for root */
		if (x->parent == x->parent->parent->left) {
			y = x->parent->parent->right;
			if (y->red) {
				x->parent->red = 0;
				y->red = 0;
				x->parent->parent->red = 1;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right) {
					x = x->parent;
					LeftRotate(x);
				}
				x->parent->red = 0;
				x->parent->parent->red = 1;
				RightRotate(x->parent->parent);
			}
		} else { /* case for x->parent == x->parent->parent->right */
			y = x->parent->parent->left;
			if (y->red) {
				x->parent->red = 0;
				y->red = 0;
				x->parent->parent->red = 1;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left) {
					x = x->parent;
					RightRotate(x);
				}
				x->parent->red = 0;
				x->parent->parent->red = 1;
				LeftRotate(x->parent->parent);
			}
		}
	}
	root->left->red = 0;
	//printf("Done: [%u %u]\n", newNode->key);

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not red in RBTreeInsert");
	assert(!root->red && "root not red in RBTreeInsert");
#endif

	return newNode;
}

bool rb_red_blk_tree::TreeInsertHelp(rb_red_blk_node* z,
		const std::vector<Range1d>& b, size_t level,
		FieldOrder_t field_order, int priority,
		rb_red_blk_node*& out_ptr) {
	/*  This function should only be called by InsertRBTree  */
	rb_red_blk_node* x;
	rb_red_blk_node* y;

	z->left = z->right = nil;
	y = root;
	x = root->left;
	while (x != nil) {
		y = x;
		int compare_result = CompareBox(x->key, z->key);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			printf("Warning compare_result == 0??\n");
			if (level != b.size() - 1) {
				x->rb_tree_next_level->RBTreeInsert(b, level + 1, field_order,
						priority);
			} else {
				//	x->node_max_priority = std::max(x->node_max_priority, priority);
				//x->nodes_priority[x->num_node_priority++] = priority;
				out_ptr = x;
			}
			delete z;
			return true;
		} else { /* x.key || z.key */
			printf("x:[%u %u], z:[%u %u]\n", x->key.low, x->key.high,
					z->key.low, z->key.high);
			printf("Warning TreeInsertHelp : x.key || z.key\n");
		}
	}
	z->parent = y;
	if ((y == root) || (1 == CompareBox(y->key, z->key))) { /* y.key > z.key */
		y->left = z;
	} else {
		y->right = z;
	}
	//found new one to insert to but will not propagate
	out_ptr = z;

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not red in TreeInsertHelp");
#endif
	return false;

}

rb_red_blk_tree::rb_red_blk_node* rb_red_blk_tree::TreeSuccessor(rb_red_blk_node* x) {
	rb_red_blk_node* y;
	if (nil != (y = x->right)) { /* assignment to y is intentional */
		while (y->left != nil) { /* returns the minium of the right subtree of x */
			y = y->left;
		}
		return y;
	} else {
		y = x->parent;
		while (x == y->right) { /* sentinel used instead of checking for nil */
			x = y;
			y = y->parent;
		}
		if (y == root)
			return nil;
		return y;
	}
}

rb_red_blk_tree::rb_red_blk_node* rb_red_blk_tree::TreePredecessor(rb_red_blk_node* x) {
	rb_red_blk_node* y;
	if (nil != (y = x->left)) { /* assignment to y is intentional */
		while (y->right != nil) { /* returns the maximum of the left subtree of x */
			y = y->right;
		}
		return y;
	} else {
		y = x->parent;
		while (x == y->left) {
			if (y == root)
				return (nil);
			x = y;
			y = y->parent;
		}
		return (y);
	}
}

void rb_red_blk_tree::InorderTreePrint(rb_red_blk_node* x) {
	// rb_red_blk_node* nil=tree->nil;
	// rb_red_blk_node* root=tree->root;
	if (x != nil) {
		InorderTreePrint(x->left);
		//printf("  key=");
		std::cout << "tree->count = " << count << std::endl;
		std::cout << x->key << std::endl;
		//  printf("  l->key=");
		// if( x->left != nil) tree->PrintKey(x->left->key);
		//  printf("  r->key=");
		//  if( x->right != nil)  tree->PrintKey(x->right->key);
		// printf("  p->key=");
		// if( x->parent != root) /*printf("NULL"); else*/ tree->PrintKey(x->parent->key);
		// printf("  red=%i\n",x->red);
		InorderTreePrint(x->right);
	}
}

void rb_red_blk_tree::TreeDestHelper(rb_red_blk_node* x) {
	if (x != nil) {
		if (x->rb_tree_next_level != nullptr)
			delete x->rb_tree_next_level;
		TreeDestHelper(x->left);
		TreeDestHelper(x->right);
		delete x;
	}
}


rb_red_blk_tree::~rb_red_blk_tree() {
	TreeDestHelper(root->left);
	delete root;
	delete nil;
}

void rb_red_blk_tree::RBTreePrint() {
	InorderTreePrint(root->left);
}

int rb_red_blk_tree::RBExactQuery(const Packet& q, size_t level,
		FieldOrder_t fieldOrder) {
	//printf("entering level %d - tree->GetMaxPriority =%d\n", level,tree->GetMaxPriority());

	//check if singleton
	if (level == fieldOrder.size()) {
		return GetMaxPriority();
	} else if (count == 1) {
		//  auto chain_boxes = tree->chain_boxes;
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1;
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1;
		}
		return GetMaxPriority();
	}

	rb_red_blk_node* x = root->left;
	int compVal;
	if (x == nil)
		return -1;
	compVal = CompareQuery(x->key, q, level, fieldOrder);
	// printf("Compval = %d\n", compVal);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nil)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}

	// printf("level = %d, priority = %d\n", level, x->mid_ptr.node_max_priority);
	return x->rb_tree_next_level->RBExactQuery(q, level + 1, fieldOrder);
}

int rb_red_blk_tree::RBExactQueryIterative(const Packet& q,
		FieldOrder_t fieldOrder, size_t level) {
	//check if singleton
	if (level == fieldOrder.size()) {
		return GetMaxPriority();
	} else if (count == 1) {
		//  auto chain_boxes = tree->chain_boxes;
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1;
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1;
		}

		return GetMaxPriority();
	}

	rb_red_blk_node* x = root->left;
	if (x == nil)
		return -1;

	int compVal = CompareQuery(x->key, q, level, fieldOrder);
	// printf("Compval = %d\n", compVal);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nil)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}
	return x->rb_tree_next_level->RBExactQueryIterative(q, fieldOrder, level + 1);

	// printf("level = %d, priority = %d\n", level, x->mid_ptr.node_max_priority);
	//return RBExactQuery(x->rb_tree_next_level, q, level + 1, fieldOrder);
}

int rb_red_blk_tree::RBExactQueryPriority(const Packet& q, size_t level,
		FieldOrder_t fieldOrder, int priority_so_far) {

	//printf("entering level %d - tree->GetMaxPriority =%d\n", level,tree->GetMaxPriority());
	if (priority_so_far > GetMaxPriority())
		return -1;
	//check if singleton
	if (level == fieldOrder.size()) {
		return GetMaxPriority();
	} else if (count == 1) {
		//  auto chain_boxes = tree->chain_boxes; 
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1;
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1;
		}
		return GetMaxPriority();
	}

	rb_red_blk_node* x = root->left;
	int compVal;
	if (x == nil)
		return -1;
	compVal = CompareQuery(x->key, q, level, fieldOrder);
	// printf("Compval = %d\n", compVal);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nil)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}

	// printf("level = %d, priority = %d\n", level, x->mid_ptr.node_max_priority);
	return x->rb_tree_next_level->RBExactQueryPriority(q, level + 1, fieldOrder,
			priority_so_far);
}

void rb_red_blk_tree::RBDeleteFixUp(rb_red_blk_node* x) {
	rb_red_blk_node* root = this->root->left;
	rb_red_blk_node* w;

	while ((!x->red) && (root != x)) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->red) {
				w->red = 0;
				x->parent->red = 1;
				LeftRotate(x->parent);
				w = x->parent->right;
			}
			if ((!w->right->red) && (!w->left->red)) {
				w->red = 1;
				x = x->parent;
			} else {
				if (!w->right->red) {
					w->left->red = 0;
					w->red = 1;
					RightRotate(w);
					w = x->parent->right;
				}
				w->red = x->parent->red;
				x->parent->red = 0;
				w->right->red = 0;
				LeftRotate(x->parent);
				x = root; /* this is to exit while loop */
			}
		} else { /* the code below is has left and right switched from above */
			w = x->parent->left;
			if (w->red) {
				w->red = 0;
				x->parent->red = 1;
				RightRotate(x->parent);
				w = x->parent->left;
			}
			if ((!w->right->red) && (!w->left->red)) {
				w->red = 1;
				x = x->parent;
			} else {
				if (!w->left->red) {
					w->right->red = 0;
					w->red = 1;
					LeftRotate(w);
					w = x->parent->left;
				}
				w->red = x->parent->red;
				x->parent->red = 0;
				w->left->red = 0;
				RightRotate(x->parent);
				x = root; /* this is to exit while loop */
			}
		}
	}
	x->red = 0;

#ifdef DEBUG_ASSERT
	assert(!nil->red && "nil not black in RBDeleteFixUp");
#endif
}

void ClearStack(std::stack<std::pair<rb_red_blk_tree *, rb_red_blk_tree::rb_red_blk_node *>>& st,
		rb_red_blk_tree* tree) {
	while (!st.empty()) {
		auto e = st.top();
		st.pop();
		if (e.first == tree) {
			delete e.second;
			return;
		}
		e.first->RBDelete(e.second);
		delete e.first;
	}

}

void rb_red_blk_tree::RBTreeDeleteWithPathCompression(rb_red_blk_tree*& tree,
		const std::vector<Range1d>& key, size_t level,
		FieldOrder_t fieldOrder, int priority,
		bool& JustDeletedTree) {
	if (level == fieldOrder.size()) {
		tree->count--;
		tree->PopPriority(priority);
		if (tree->count == 0) {
			delete tree;
			JustDeletedTree = true;
		}
		return;
	}
	if (tree->count == 1) {
		if (level == 0) {
			tree->count = 0;
			tree->chain_boxes.clear();
			tree->ClearPriority();
		} else {
			delete tree;
			JustDeletedTree = true;
		}
		return;
	}
	if (tree->count == 2) {
		int run = 0;
		rb_red_blk_tree * temp_tree = tree;

		//first time tree->count ==2; this mean you need to create chain box here;
		//keep going until you find the node that contians tree->count = 1
		//	std::stack<std::pair<rb_red_blk_tree *, rb_red_blk_node *>> stack_so_far;

		while (true) {
			if (temp_tree->count == 1 || level + run == fieldOrder.size()) {
				tree->chain_boxes.insert(end(tree->chain_boxes),
						begin(temp_tree->chain_boxes),
						end(temp_tree->chain_boxes));

				auto newtree = new rb_red_blk_tree();
				newtree->chain_boxes = tree->chain_boxes;

				if (temp_tree->GetSizeList() == 2) {
					temp_tree->PopPriority(priority);
				}
				int new_priority = temp_tree->GetMaxPriority();
				delete tree;

				tree = newtree;
				tree->count = 1;
				tree->PushPriority(new_priority);
				return;
			}
			/*if (level + run == fieldOrder.size()) {
			 auto newtree = RBTreeCreate();
			 newtree->chain_boxes = tree->chain_boxes;
			 RBTreeDestroy(tree);
			 tree = newtree;
			 tree->count = 1;
			 if (temp_tree->GetSizeList() == 2) {
			 temp_tree->PopPriority(priority);
			 }
			 tree->PushPriority(temp_tree->GetMaxPriority());

			 //	RBTreeDestroy(temp_tree);
			 //ClearStack(stack_so_far, tree);
			 //tree->PopPriority(priority);
			 return ;
			 }*/
			temp_tree->count--;
			rb_red_blk_node * x = temp_tree->root->left;
			if (x->left == temp_tree->nil && x->right == temp_tree->nil) {
				tree->chain_boxes.push_back(x->key);
				//stack_so_far.push(std::make_pair(temp_tree, x));
				temp_tree = x->rb_tree_next_level;
			} else {
				int compare_result = CompareBox(x->key,
						key[fieldOrder[level + run]]);
				//stack_so_far.push(std::make_pair(temp_tree, x));
				if (compare_result == 0) { //hit top = delete top then go leaf node to collect correct chain box
					if (x->left == temp_tree->nil) {
						temp_tree = x->right->rb_tree_next_level;
						tree->chain_boxes.push_back(x->right->key);
					} else {
						temp_tree = x->left->rb_tree_next_level;
						tree->chain_boxes.push_back(x->left->key);
					}
					//	tree->chain_boxes.push_back(x->left == temp_tree->nil?x->right->key:x->left->key);
					//	temp_tree = x->rb_tree_next_level;
				} else {
					temp_tree = x->rb_tree_next_level;
					tree->chain_boxes.push_back(x->key);
				}

				/*if (compare_result == -1){  root < z.key 
				 //delete right child
				 //stack_so_far.push(std::make_pair(temp_tree, x->right));
				 tree->chain_boxes.push_back( x->key);
				 temp_tree = x->right->rb_tree_next_level;
				 } else if(compare_result == 1) {
				 //delete left child
				 //stack_so_far.push(std::make_pair(temp_tree, x->left));
				 temp_tree = x->left->rb_tree_next_level;
				 tree->chain_boxes.push_back(x->key);
				 } else {
				 printf("Warning::RBTreeDeleteWithPathCompression Overlap at level %d\n",level+run);
				 }*/

			}
			run++;

		}
		return;
	}

	rb_red_blk_node* x;
	//rb_red_blk_node* y;
	rb_red_blk_node* nil = tree->nil;
	//y = tree->root;
	x = tree->root->left;

	while (x != nil) {
		//y = x;
		int compare_result = CompareBox(x->key, key[fieldOrder[level]]);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			bool justDelete = false;
			tree->count--;
			RBTreeDeleteWithPathCompression(x->rb_tree_next_level, key,
					level + 1, fieldOrder, priority, justDelete);
			if (justDelete)
				tree->RBDelete(x);

			return;

		} else { /* x.key || z.key */
			printf("x:[%u %u], key:[%u %u]\n", x->key.low, x->key.high,
					key[fieldOrder[level]].low,
					key[fieldOrder[level]].high);
			printf(
					"Warning RBFindNodeSequence : x.key || key[fieldOrder[level]]\n");
		}
	}

	printf(
			"Error RBTreeDeleteWithPathCompression: can't find a node at level %zd\n",
			level);
	exit(0);

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

void rb_red_blk_tree::RBDelete(rb_red_blk_node* z) {
	rb_red_blk_node* y;
	rb_red_blk_node* x;
	rb_red_blk_node* nil = this->nil;
	rb_red_blk_node* root = this->root;

	y = ((z->left == nil) || (z->right == nil)) ? z : TreeSuccessor(z);
	x = (y->left == nil) ? y->right : y->left;
	if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
		root->left = x;
	} else {
		if (y == y->parent->left) {
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	}
	if (y != z) { /* y should not be nil in this case */

#ifdef DEBUG_ASSERT
		assert((y != this->nil) && "y is nil in RBDelete\n");
#endif
		/* y is the node to splice out and x is its child */

		if (!(y->red))
			RBDeleteFixUp(x);

		//  tree->DestroyKey(z->key);
		//  tree->DestroyInfo(z->info);
		y->left = z->left;
		y->right = z->right;
		y->parent = z->parent;
		y->red = z->red;
		z->left->parent = z->right->parent = y;
		if (z == z->parent->left) {
			z->parent->left = y;
		} else {
			z->parent->right = y;
		}
		free(z);
	} else {
//    tree->DestroyKey(y->key);
		//   tree->DestroyInfo(y->info);
		if (!(y->red))
			RBDeleteFixUp(x);
		delete y;
	}

#ifdef DEBUG_ASSERT
	assert(!this->nil->red && "nil not black in RBDelete");
#endif
}

std::stack<void *> * rb_red_blk_tree::RBEnumerate(
		const Range1d& low, const Range1d& high) {
	auto enumResultStack = new std::stack<void *>;
	rb_red_blk_node* nil = this->nil;
	rb_red_blk_node* x = this->root->left;
	rb_red_blk_node* lastBest = nil;

	while (nil != x) {
		if (1 == (CompareBox(x->key, high))) { /* x->key > high */
			x = x->left;
		} else {
			lastBest = x;
			x = x->right;
		}
	}
	while ((lastBest != nil) && (1 != CompareBox(low, lastBest->key))) {
		enumResultStack->push(lastBest);
		lastBest = TreePredecessor(lastBest);
	}
	return (enumResultStack);
}

/**********************************************************************
 *  FUNCTION:  RBSerializeIntoRulesRecursion
 *
 *    INPUTS:  tree: 'tree' at 'level' for current 'fieldOrder'
 *
 *
 *    OUTPUT:  a vector of Rule
 *
 *    Modifies Input: none
 ***********************************************************************/

void rb_red_blk_tree::RBSerializeIntoRulesRecursion(rb_red_blk_node * node, size_t level,
		FieldOrder_t fieldOrder, std::vector<Range1d>& box_so_far,
		std::vector<Rule>& rules_so_far) {
	if (level == fieldOrder.size()) {
		for (int n : this->priority_list) {
			Rule r(fieldOrder.size());
			for (int i = 0; i < r.dim; i++) {
				r.range[fieldOrder[i]] = box_so_far[i];
			}
			r.priority = n;
			rules_so_far.push_back(r);
		}
		return;
	}
	if (this->count == 1) {
		box_so_far.insert(std::end(box_so_far), begin(this->chain_boxes),
				end(this->chain_boxes));
		for (int n : this->priority_list) {
			Rule r(fieldOrder.size());

			for (int i = 0; i < r.dim; i++) {
				r.range[fieldOrder[i]] = box_so_far[i];
			}
			r.priority = n;
			rules_so_far.push_back(r);
		}

		for (size_t i = 0; i < this->chain_boxes.size(); i++)
			box_so_far.pop_back();

		return;
	}

	if (this->nil == node)
		return;

	box_so_far.push_back(node->key);
	auto tree = node->rb_tree_next_level;
	tree->RBSerializeIntoRulesRecursion(tree->root->left, level + 1, fieldOrder,
			box_so_far, rules_so_far);
	box_so_far.pop_back();

	RBSerializeIntoRulesRecursion(node->left, level, fieldOrder,
			box_so_far, rules_so_far);
	RBSerializeIntoRulesRecursion(node->right, level, fieldOrder,
			box_so_far, rules_so_far);

}

std::vector<Rule> rb_red_blk_tree::RBSerializeIntoRules(
		FieldOrder_t fieldOrder) {
	std::vector<Range1d> boxes_so_far;
	std::vector<Rule> rules_so_far;
	RBSerializeIntoRulesRecursion(this->root->left, 0, fieldOrder,
			boxes_so_far, rules_so_far);
	return rules_so_far;
}

/******************/

//Practice of self-documenting codes.
int rb_red_blk_tree::CalculateMemoryConsumptionRecursion(
		rb_red_blk_node * node, size_t level,
		FieldOrder_t fieldOrder) {
	if (level == fieldOrder.size()) {

		int num_rules_in_this_leaf = this->priority_list.size();
		int sizeofint = 4;
		int also_max_priority = 1;
		return (num_rules_in_this_leaf + also_max_priority) * sizeofint;

	}
	if (this->count == 1) {
		int size_of_remaining_intervals = 0;
		for (size_t i = level; i < fieldOrder.size(); i++) {
			int field = fieldOrder[i];
			int intervalsize;
			if (field == 0 || field == 1) {
				intervalsize = 9;
			}
			if (field == 2 || field == 3) {
				intervalsize = 4;
			}
			if (field == 4) {
				intervalsize = 1;
			}
			size_of_remaining_intervals += intervalsize;
		}
		int num_rules_in_this_leaf = this->priority_list.size();
		int sizeofint = 4;
		int also_max_priority = 1;

		return size_of_remaining_intervals
				+ (num_rules_in_this_leaf + also_max_priority) * sizeofint;
	}

	if (this->nil == node)
		return 0;

	int memory_usage = 0;
	auto tree = node->rb_tree_next_level;

	memory_usage += tree->CalculateMemoryConsumptionRecursion(tree->root->left,
			level + 1, fieldOrder);
	memory_usage += CalculateMemoryConsumptionRecursion(
			node->left, level, fieldOrder);
	memory_usage += CalculateMemoryConsumptionRecursion(
			node->right, level, fieldOrder);

	int number_pointers_in_internal_node = 4;
	int aux_data_internal_node_byte = 1;
	int sizeofint = 4;
	int also_max_priority_interval_node = 1;

	int field = fieldOrder[level] % 5;
	int intervalsize;
	if (field == 0 || field == 1) {
		intervalsize = 9;
	}
	if (field == 2 || field == 3) {
		intervalsize = 4;
	}
	if (field == 4) {
		intervalsize = 1;
	}
	return memory_usage + intervalsize
			+ (number_pointers_in_internal_node
					+ also_max_priority_interval_node) * sizeofint
			+ aux_data_internal_node_byte;
}

int rb_red_blk_tree::CalculateMemoryConsumption(FieldOrder_t fieldOrder) {
	return CalculateMemoryConsumptionRecursion(this->root->left, 0,
			fieldOrder);
}
