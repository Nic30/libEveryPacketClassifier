# FaR
* is optimized PartitionSort for hardware architectures
* PartitionSort uses multiple decision trees divided in to levels.
  Each level is composed of red-black trees and performs the classification for single dimension


# Red-black tree
* approximately balanced binary tree
* https://www.cs.usfca.edu/~galles/visualization/RedBlack.html
* implementation is modified https://github.com/anandarao/Red-Black-Tree 
* The root is always black.
* Leaf nodes are black.
* If a node is red, then both its children are black.
* The number of black nodes on every path are same.


## properties
* O(log n) max path len, insert/delete/lookup T.


## special operations
* left/right rotation - restores properties of rb tree.
```
   B                                C
 |   |       ---left rot. -->     |   |
A     C                           B    E
    |   |    <--right rot. --   |   |
    D   E                       A   D
```
A < B < D < C < E


# Path compression
* http://theoryofprogramming.com/2016/11/15/compressed-trie-tree/


# parallel FaR insert algorithm
```
input: vector of rules (batch)

// it is expected that the majority of the rules will have known group
// as the number of groups is much smaller than the number of rules
for each rule check the mask_to_tree
  if there is a known tree for this type of rule

// mask is the heuristic for the rule insertion
with threadpool
check if the rule does fit in the tree specified by mask

// it is expected that the majority of the rules will be resolved in first iteration
// only the rules from the non discovered groups may require multiple iterations
while true:
    // we can not check all of them first because tree will change after insert
    // and this may lead to suboptimal trees
	with threadpool
	for amount of rules which does not have the tree resolved
	  check if fits to any tree 
	
	for rule wihich got the tree resolved
	  insert rule in to the best matching tree
	  update the mask_to_tree
	 
	if all rules maped
	  return
```
