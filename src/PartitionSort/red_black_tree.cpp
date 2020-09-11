#include "red_black_tree.h"
#include <assert.h>
// https://bitbucket.org/bcsaller/rbtree/src/ee44581da45cd33a0a33a2012392de9a568a3122/src/rbtree_impl.c?at=default&fileviewer=file-view-default

void RedBlackTree::pushPriority(int p) {
	max_priority_local = std::max(p, max_priority_local);
	priority_list.push_back(p);
}

void RedBlackTree::popPriority(int p) {
	find(begin(priority_list), end(priority_list), p);
	priority_list.erase(find(begin(priority_list), end(priority_list), p));
	if (p == max_priority_local) {
		max_priority_local = *std::max_element(begin(priority_list),
				end(priority_list));
	}
	if (priority_list.empty())
		max_priority_local = -1;
}

void RedBlackTree::clearPriority() {
	max_priority_local = -1;
	priority_list.clear();
}

int RedBlackTree::getMaxPriority() const {
	return max_priority_local;
}

int RedBlackTree::getSizeList() const {
	return priority_list.size();
}

int inline CompareQuery(const Range1d& a, const Packet& q, size_t level,
		RedBlackTree::FieldOrder_t fieldOrder) {
	if (a.high < q[fieldOrder[level]]) {
		return -1;
	} else if (a.low > q[fieldOrder[level]]) {
		return 1;
	}

	return 0;
}

RedBlackTree::RedBlackTree() {
	max_priority_local = -1;
	this->root = nullptr;
}

void RedBlackTree::rotateLeft(RedBlackTree_node* p) {
	if (p->right == nullptr)
		return;
	else {
		auto y = p->right;
		if (y->left) {
			p->right = y->left;
			y->left->parent = p;
		} else
			p->right = nullptr;
		if (p->parent)
			y->parent = p->parent;

		if (p->parent == nullptr)
			root = y;
		else {
			if (p == p->parent->left)
				p->parent->left = y;
			else
				p->parent->right = y;
		}
		y->left = p;
		p->parent = y;
	}
}

void RedBlackTree::rotateRight(RedBlackTree_node* p) {
	if (p->left == nullptr)
		return;
	else {
		auto y = p->left;
		if (y->right) {
			p->left = y->right;
			y->right->parent = p;
		} else
			p->left = nullptr;
		if (p->parent)
			y->parent = p->parent;
		if (p->parent == nullptr)
			root = y;
		else {
			if (p == p->parent->left)
				p->parent->left = y;
			else
				p->parent->right = y;
		}
		y->right = p;
		p->parent = y;
	}
}

bool RedBlackTree::canInsert(const std::vector<Range1d>& z, size_t level,
		FieldOrder_t fieldOrder) {

	if (level == fieldOrder.size()) {
		// can insert into rule list
		return true;
	} else if (count == 1) {
		// this node has only one children
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (z[fieldOrder[i]] == chain_boxes[i - level])
				continue;

			if (z[fieldOrder[i]].isIntersect(chain_boxes[i - level]))
				return false;
			else
				return true;
		}
		return true;
	}

	RedBlackTree_node* x = root;
	while (x) {
		int compare_result = x->key.cmp(z[fieldOrder[level]]);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			if (level == z.size() - 1 || x->rb_tree_next_level == nullptr)
				return true;
			else
				return x->rb_tree_next_level->canInsert(z, level + 1,
						fieldOrder);
		} else { // intersecting
			return false;
		}
	}
	return true;
}

bool RedBlackTree::insertWithPathCompressionHelp(RedBlackTree_node* z,
		const std::vector<Range1d>& b, size_t level, FieldOrder_t fieldOrder,
		int priority, RedBlackTree_node*& out_ptr) {
	/*  This function should only be called by InsertRBTree (see above) */
	RedBlackTree_node* x;
	RedBlackTree_node* y;

	out_ptr = z->left = z->right = nullptr;
	y = x = root;
	while (x) {
		y = x;
		int compare_result = x->key.cmp(z->key);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			if (level != fieldOrder.size() - 1) {
				x->rb_tree_next_level->insertWithPathCompression(b, level + 1,
						fieldOrder, priority);
			} else {
				if (x->rb_tree_next_level == nullptr)
					x->rb_tree_next_level = new RedBlackTree();
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->pushPriority(priority);
				out_ptr = x;
			}
			return true;
		} else { /* x.key || z.key */
			// inserting colliding key
			printf("Warning TreeInsertPathcompressionHelp : x.key || z.key\n");
		}
	}
	z->parent = y;
	if (root == nullptr) {
		root = z;
	} else if (y == root || y->key > z->key) {
		y->left = z;
	} else {
		y->right = z;
	}
	//found new one to insert to
	//need to create a tree first then followed by insertion
	//we use path-compression here since this tree contains a single rule
	z->rb_tree_next_level = new RedBlackTree();
	z->rb_tree_next_level->insertWithPathCompression(b, level + 1, fieldOrder,
			priority);

	return false;
}

RedBlackTree::RedBlackTree_node * RedBlackTree::insertWithPathCompression(
		const std::vector<Range1d>& key, size_t level, FieldOrder_t fieldOrder,
		int priority) {


	if (level == fieldOrder.size()) {
		count++;
		pushPriority(priority);
		return nullptr;
	}

	if (count == 0) {
		//no need to create a node yet; just compress the path
		count++;
		pushPriority(priority);
		for (size_t i = level; i < fieldOrder.size(); i++)
			chain_boxes.push_back(key[fieldOrder[i]]);
		return nullptr;
	}

	if (count == 1) {
		//path compression
		auto temp_chain_boxes = chain_boxes; // copy of vector
		int xpriority = getMaxPriority();

		count++;

		//quick check if identical just reset count = 1;
		chain_boxes.clear();

		// unzipping the next level
		std::vector<int> naturalFieldOrder(fieldOrder.size());
		std::iota(begin(naturalFieldOrder), end(naturalFieldOrder), 0);
		size_t run = 0;
		if (temp_chain_boxes[run] == key[fieldOrder[level + run]]) {
			RedBlackTree_node * x = insert(key, level + run++, fieldOrder, xpriority);
			if (level + run < fieldOrder.size()) {
				while ((temp_chain_boxes[run] == key[fieldOrder[level + run]])) {
					x->rb_tree_next_level = new RedBlackTree();
					x->rb_tree_next_level->count = 2;
					x = x->rb_tree_next_level->insert(key, level + run,
							fieldOrder, priority);

					run++;

					if (level + run >= fieldOrder.size())
						break;
				}
			}
			if (level + run >= fieldOrder.size()) {
				x->rb_tree_next_level = new RedBlackTree();
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->pushPriority(priority);
				x->rb_tree_next_level->count++;
				x->rb_tree_next_level->pushPriority(xpriority);
			} else if (!(temp_chain_boxes[run] == key[fieldOrder[level + run]])) {
				if (temp_chain_boxes[run].isIntersect(key[fieldOrder[level + run]])) {
					std::cout << "Warning not intersect?" << std::endl;
					std::cout << temp_chain_boxes[run] << " vs. " <<
							key[fieldOrder[level + run]] << std::endl;
					std::cout << "chain_boxes:" << std::endl;
					for (auto e : temp_chain_boxes)
						std::cout << e;
					std::cout << std::endl << " boxes:" << std::endl;
					for (size_t i = 0; i < key.size(); i++) {
						std::cout << key[fieldOrder[i]];
					}
					std::cout << std::endl;
					throw std::runtime_error("not intersect");
				}
				//split into z and x node
				x->rb_tree_next_level = new RedBlackTree();
				auto PrependChainbox =
						[](std::vector<Range1d>& cb, int n_prepend) {
							std::vector<Range1d> t;
							for (int i = 0; i < n_prepend; i++)
								t.push_back( {999, 100020});
							t.insert(end(t), begin(cb), end(cb));
							return t;
						};
				auto z1 = x->rb_tree_next_level->insert(
						PrependChainbox(temp_chain_boxes, level), level + run,
						naturalFieldOrder, xpriority);
				auto z2 = x->rb_tree_next_level->insert(key, level + run,
						fieldOrder, priority);

				x->rb_tree_next_level->count = 2;

				z1->rb_tree_next_level = new RedBlackTree();
				z2->rb_tree_next_level = new RedBlackTree();
				z1->rb_tree_next_level->insertWithPathCompression(
						PrependChainbox(temp_chain_boxes, level),
						level + run + 1, naturalFieldOrder, xpriority);
				z2->rb_tree_next_level->insertWithPathCompression(key,
						level + run + 1, fieldOrder, priority);

			}
		} else {

			auto PrependChainbox =
					[](std::vector<Range1d>& cb, int n_prepend) {
						std::vector<Range1d> t;
						for (int i = 0; i < n_prepend; i++)
							t.push_back( {0, 10000000});
						t.insert(end(t), begin(cb), end(cb));
						return t;
					};

			auto z1 = insert(PrependChainbox(temp_chain_boxes, level),
					level + run, naturalFieldOrder, xpriority);
			auto z2 = insert(key, level + run, fieldOrder, priority);
			count = 2;

			z1->rb_tree_next_level = new RedBlackTree();
			z2->rb_tree_next_level = new RedBlackTree();

			z1->rb_tree_next_level->insertWithPathCompression(
					PrependChainbox(temp_chain_boxes, level), level + run + 1,
					naturalFieldOrder, xpriority);
			z2->rb_tree_next_level->insertWithPathCompression(key,
					level + run + 1, fieldOrder, priority);

		}
		return nullptr;
	}

	count++;

	RedBlackTree_node * x = new RedBlackTree_node;
	x->key = key[fieldOrder[level]];
	RedBlackTree_node * out_ptr;
	if (insertWithPathCompressionHelp(x, key, level, fieldOrder, priority,
			out_ptr)) {
		delete x;
		//insertion finds identical box.
		//do nothing for now
		return out_ptr;
	}

	_insertFix(x);

	return x;
}

void RedBlackTree::_insertFix(RedBlackTree_node * t) {
	RedBlackTree_node *u;
	if (root == t) {
		t->red = 0;
		return;
	}
	while (t->parent != nullptr && t->parent->red) {
		auto g = t->parent->parent;
		if (g->left == t->parent) {
			if (g->right != NULL) {
				u = g->right;
				if (u->red) {
					t->parent->red = 0;
					u->red = 0;
					g->red = 1;
					t = g;
				}
			} else {
				if (t->parent->right == t) {
					t = t->parent;
					rotateLeft(t);
				}
				t->parent->red = 0;
				g->red = 1;
				rotateRight(g);
			}
		} else {
			if (g->left != NULL) {
				u = g->left;
				if (u->red) {
					t->parent->red = 0;
					u->red = 0;
					g->red = 1;
					t = g;
				}
			} else {
				if (t->parent->left == t) {
					t = t->parent;
					rotateRight(t);
				}
				t->parent->red = 0;
				g->red = 1;
				rotateLeft(g);
			}
		}
		root->red = 0;
	}
}

RedBlackTree::RedBlackTree_node * RedBlackTree::insert(
		const std::vector<Range1d>& key, size_t level, FieldOrder_t field_order,
		int priority) {

	if (level == key.size())
		return nullptr;


	RedBlackTree_node * x = new RedBlackTree_node;
	x->key = key[field_order[level]];
	RedBlackTree_node * out_ptr;
	if (insert_and_check_if_exists(x, key, level, field_order, priority, out_ptr)) {
		//insertion finds identical box. do nothing for now
		delete x;
		return out_ptr;
	}

	_insertFix(x);

	return x;
}

bool RedBlackTree::insert_and_check_if_exists(RedBlackTree_node* z,
		const std::vector<Range1d>& b, size_t level, FieldOrder_t field_order,
		int priority, RedBlackTree_node*& out_ptr) {
	/*  This function should only be called by InsertRBTree  */
	RedBlackTree_node* x;
	RedBlackTree_node* y;

	z->left = z->right = nullptr;
	y = x = root;
	while (x) {
		y = x;
		int compare_result = x->key.cmp(z->key);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			printf("Warning compare_result == 0??\n"); // reinserting a key
			if (level != b.size() - 1) {
				x->rb_tree_next_level->insert(b, level + 1, field_order,
						priority);
			} else {
				out_ptr = x;
			}
			return true;
		} else { /* x.key || z.key */
			std::cout << "x:" << x->key << ", z:" << z->key << std::endl;
			printf("Warning TreeInsertHelp : x.key || z.key\n");
		}
	}
	z->parent = y;
	if ((y == root) || (1 == y->key.cmp(z->key))) { /* y.key > z.key */
		if (root == nullptr)
			root = z;
		else
			y->left = z;
	} else {
		y->right = z;
	}
	//found new one to insert to but will not propagate
	out_ptr = z;

	return false;
}

RedBlackTree::RedBlackTree_node* RedBlackTree::getSuccessor(
		RedBlackTree_node* x) {
	RedBlackTree_node* y;
	if (x->left) {
		y = x->left;
		// the maximum of the right subtree of x
		while (y->right)
			y = y->right;
	} else {
		y = x->right;
		// the minimum of left sub tree
		while (y->left)
			y = y->left;
	}
    return y;
}

RedBlackTree::RedBlackTree_node* RedBlackTree::getPredecessor(
		RedBlackTree_node* x) {
	RedBlackTree_node* y;
	if (x->right) {
		y = x->right;
		// the minimum of the right subtree of x
		while (y->left)
			y = y->left;
	} else {
		y = x->left;
		// the maximum of right sub tree
		while (y->right)
			y = y->right;
	}
    return y;
}

void RedBlackTree::printInorder(RedBlackTree_node* x) {
	if (x) {
		printInorder(x->left);
		std::cout << "tree->count = " << count << std::endl;
		std::cout << x->key << std::endl;
		printInorder(x->right);
	}
}

void RedBlackTree::destHelper(RedBlackTree_node* x) {
	if (x) {
		if (x->rb_tree_next_level != nullptr)
			delete x->rb_tree_next_level;
		destHelper(x->left);
		destHelper(x->right);
		delete x;
	}
}

RedBlackTree::~RedBlackTree() {
	destHelper(root);
	root = nullptr;
}

void RedBlackTree::print() {
	printInorder(root);
}

int RedBlackTree::exactQuery(const Packet& q, size_t level,
		FieldOrder_t fieldOrder) {

	//check if singleton
	if (level == fieldOrder.size()) {
		return getMaxPriority();
	} else if (count == 1) {
		//  auto chain_boxes = tree->chain_boxes;
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1;
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1;
		}
		return getMaxPriority();
	}

	RedBlackTree_node* x = root;
	if (x == nullptr)
		return -1;
	int compVal = CompareQuery(x->key, q, level, fieldOrder);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nullptr)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}

	return x->rb_tree_next_level->exactQuery(q, level + 1, fieldOrder);
}

int RedBlackTree::exactQueryIterative(const Packet& q, FieldOrder_t fieldOrder,
		size_t level) {
	//check if singleton
	if (level == fieldOrder.size()) {
		return getMaxPriority(); // default rule
	} else if (count == 1) { // use chain boxes
		//  auto chain_boxes = tree->chain_boxes;
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1; // not found
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1; // not found
		}

		return getMaxPriority();
	}
	// wal the RedBlack tree on current node
	RedBlackTree_node* x = root;
	if (x == nullptr)
		return -1;

	int compVal = CompareQuery(x->key, q, level, fieldOrder);
	// printf("Compval = %d\n", compVal);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nullptr)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}
	// after the leaf of RedBlack tree on current level is fund use it to search further
	return x->rb_tree_next_level->exactQueryIterative(q, fieldOrder, level + 1);
}

int RedBlackTree::exactQueryPriority(const Packet& q, size_t level,
		FieldOrder_t fieldOrder, int priority_so_far) {

	//printf("entering level %d - tree->GetMaxPriority =%d\n", level,tree->GetMaxPriority());
	if (priority_so_far > getMaxPriority())
		return -1;
	//check if singleton
	if (level == fieldOrder.size()) {
		return getMaxPriority();
	} else if (count == 1) {
		//  auto chain_boxes = tree->chain_boxes; 
		for (size_t i = level; i < fieldOrder.size(); i++) {
			if (q[fieldOrder[i]] < chain_boxes[i - level].low)
				return -1;
			if (q[fieldOrder[i]] > chain_boxes[i - level].high)
				return -1;
		}
		return getMaxPriority();
	}

	RedBlackTree_node* x = root;
	int compVal;
	if (x == nullptr)
		return -1;
	compVal = CompareQuery(x->key, q, level, fieldOrder);
	// printf("Compval = %d\n", compVal);
	while (0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > q */
			x = x->left;
		} else if (-1 == compVal) /*x->key < q */{
			x = x->right;
		}
		if (x == nullptr)
			return -1;
		compVal = CompareQuery(x->key, q, level, fieldOrder);
	}

	// printf("level = %d, priority = %d\n", level, x->mid_ptr.node_max_priority);
	return x->rb_tree_next_level->exactQueryPriority(q, level + 1, fieldOrder,
			priority_so_far);
}

void RedBlackTree::deleteFixUp(RedBlackTree_node* p) {
	RedBlackTree_node *s;
	while (p != root && !p->red) {
		if (p->parent->left == p) {
			s = p->parent->right;
			if (s->red) {
				s->red = 0;
				p->parent->red = 1;
				rotateLeft(p->parent);
				s = p->parent->right;
			}
			if (!s->right->red && !s->left->red) {
				s->red = 1;
				p = p->parent;
			} else {
				if (!s->right->red) {
					s->red = 1;
					s->left->red = 0;
					rotateRight(s);
					s = p->parent->right;
				}
				s->red = p->parent->red;
				p->parent->red = 0;
				s->right->red = 0;
				rotateLeft(p->parent);
				p = root;
			}
		} else {
			s = p->parent->left;
			if (s->red) {
				s->red = 0;
				p->parent->red = 1;
				rotateRight(p->parent);
				s = p->parent->left;
			}
			if (!s->left->red && !s->right->red) {
				s->red = 1;
				p = p->parent;
			} else {
				if (!s->left->red) {
					s->right->red = 0;
					s->red = 1;
					rotateLeft(s);
					s = p->parent->left;
				}
				s->red = p->parent->red;
				p->parent->red = 0;
				s->left->red = 0;
				rotateRight(p->parent);
				p = root;
			}
		}
		p->red = 0;
		root->red = 0;
	}
}

void ClearStack(
		std::stack<std::pair<RedBlackTree *, RedBlackTree::RedBlackTree_node *>>& st,
		RedBlackTree* tree) {
	while (!st.empty()) {
		auto e = st.top();
		st.pop();
		if (e.first == tree) {
			delete e.second;
			return;
		}
		e.first->deleteNode(e.second);
		delete e.first;
	}

}

void RedBlackTree::deleteWithPathCompression(RedBlackTree*& tree,
		const std::vector<Range1d>& key, size_t level, FieldOrder_t fieldOrder,
		int priority, bool& JustDeletedTree) {
	if (level == fieldOrder.size()) {
		tree->count--;
		tree->popPriority(priority);
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
			tree->clearPriority();
		} else {
			delete tree;
			JustDeletedTree = true;
		}
		return;
	}
	if (tree->count == 2) {
		int run = 0;
		RedBlackTree * temp_tree = tree;

		//first time tree->count ==2; this mean you need to create chain box here;
		//keep going until you find the node that contians tree->count = 1
		while (true) {
			if (temp_tree->count == 1 || level + run == fieldOrder.size()) {
				tree->chain_boxes.insert(end(tree->chain_boxes),
						begin(temp_tree->chain_boxes),
						end(temp_tree->chain_boxes));

				auto newtree = new RedBlackTree();
				newtree->chain_boxes = tree->chain_boxes;

				if (temp_tree->getSizeList() == 2) {
					temp_tree->popPriority(priority);
				}
				int new_priority = temp_tree->getMaxPriority();
				delete tree;

				tree = newtree;
				tree->count = 1;
				tree->pushPriority(new_priority);
				return;
			}
			temp_tree->count--;
			RedBlackTree_node * x = temp_tree->root;
			if (x->left == nullptr && x->right == nullptr) {
				tree->chain_boxes.push_back(x->key);
				//stack_so_far.push(std::make_pair(temp_tree, x));
				temp_tree = x->rb_tree_next_level;
			} else {
				int compare_result = x->key.cmp(key[fieldOrder[level + run]]);
				if (compare_result == 0) { //hit top = delete top then go leaf node to collect correct chain box
					if (x->left == nullptr) {
						temp_tree = x->right->rb_tree_next_level;
						tree->chain_boxes.push_back(x->right->key);
					} else {
						temp_tree = x->left->rb_tree_next_level;
						tree->chain_boxes.push_back(x->left->key);
					}
				} else {
					temp_tree = x->rb_tree_next_level;
					tree->chain_boxes.push_back(x->key);
				}
			}
			run++;

		}
		return;
	}

	RedBlackTree_node* x;
	x = tree->root;

	while (x) {
		//y = x;
		int compare_result = x->key.cmp(key[fieldOrder[level]]);
		if (compare_result == 1) { /* x.key > z.key */
			x = x->left;
		} else if (compare_result == -1) { /* x.key < z.key */
			x = x->right;
		} else if (compare_result == 0) { /* x.key = z.key */
			bool justDelete = false;
			tree->count--;
			deleteWithPathCompression(x->rb_tree_next_level, key, level + 1,
					fieldOrder, priority, justDelete);
			if (justDelete)
				tree->deleteNode(x);

			return;

		} else { /* x.key || z.key */
			std::cout << "x:"<< x->key << ", key:" << key[fieldOrder[level]] << std::endl;
			printf(
					"Warning RBFindNodeSequence : x.key || key[fieldOrder[level]]\n");
		}
	}

	throw std::runtime_error(
			std::string("Error RBTreeDeleteWithPathCompression: can't find a node at level ") + std::to_string(level));
}

void RedBlackTree::deleteNode(RedBlackTree_node* z) {
	RedBlackTree_node* y;
	RedBlackTree_node* x;
	RedBlackTree_node* root = this->root;

	y = ((z->left == nullptr) || (z->right == nullptr)) ? z : getSuccessor(z);
	x = (y->left == nullptr) ? y->right : y->left;
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
		/* y is the node to splice out and x is its child */

		if (!(y->red))
			deleteFixUp(x);

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
		delete z;
	} else {
		if (!(y->red))
			deleteFixUp(x);
		delete y;
	}
}

std::stack<RedBlackTree::RedBlackTree_node*> * RedBlackTree::RBEnumerate(
		const Range1d& low, const Range1d& high) {
	auto enumResultStack = new std::stack<RedBlackTree_node*>();
	RedBlackTree_node* x = this->root->left;
	RedBlackTree_node* lastBest = nullptr;

	while (x) {
		if (1 == (x->key.cmp(high))) { /* x->key > high */
			x = x->left;
		} else {
			lastBest = x;
			x = x->right;
		}
	}
	while (lastBest && (1 != low.cmp(lastBest->key))) {
		enumResultStack->push(lastBest);
		lastBest = getPredecessor(lastBest);
	}
	return enumResultStack;
}

void RedBlackTree::serializeIntoRulesRecursion(RedBlackTree_node * node,
		size_t level, FieldOrder_t fieldOrder, std::vector<Range1d>& box_so_far,
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

	if (node == nullptr)
		return;

	box_so_far.push_back(node->key);
	auto tree = node->rb_tree_next_level;
	tree->serializeIntoRulesRecursion(tree->root, level + 1, fieldOrder,
			box_so_far, rules_so_far);
	box_so_far.pop_back();

	serializeIntoRulesRecursion(node->left, level, fieldOrder, box_so_far,
			rules_so_far);
	serializeIntoRulesRecursion(node->right, level, fieldOrder, box_so_far,
			rules_so_far);

}

std::vector<Rule> RedBlackTree::serializeIntoRules(FieldOrder_t fieldOrder) {
	std::vector<Range1d> boxes_so_far;
	std::vector<Rule> rules_so_far;
	serializeIntoRulesRecursion(this->root->left, 0, fieldOrder, boxes_so_far,
			rules_so_far);
	return rules_so_far;
}

int RedBlackTree::calculateMemoryConsumptionRecursion(RedBlackTree_node * node,
		size_t level, FieldOrder_t fieldOrder) {

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
		int also_max_priority = 1;

		return size_of_remaining_intervals
				+ (num_rules_in_this_leaf + also_max_priority) * sizeof(unsigned);
	}

	if (node == nullptr)
		return 0;

	int memory_usage = 0;
	auto tree = node->rb_tree_next_level;

	memory_usage += tree->calculateMemoryConsumptionRecursion(tree->root->left,
			level + 1, fieldOrder);
	memory_usage += calculateMemoryConsumptionRecursion(node->left, level,
			fieldOrder);
	memory_usage += calculateMemoryConsumptionRecursion(node->right, level,
			fieldOrder);

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

int RedBlackTree::calculateMemoryConsumption(FieldOrder_t fieldOrder) {
	return calculateMemoryConsumptionRecursion(this->root->left, 0, fieldOrder);
}
