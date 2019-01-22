# Red-black tree
* approximately balanced binary tree
* https://www.cs.usfca.edu/~galles/visualization/RedBlack.html
* implementation is modified https://github.com/anandarao/Red-Black-Tree 
* The root is always black.
* Leaf nodes are black.
* If a node is red, then both its children are black.
* The number of black nodes on every path are same.


# Left Larning Red-black tree
Left-Leaning Red-Black (LLRB) is an implementation of  2–3 balanced binary search tree.
“Left-leaning Red-Black Trees” by Robert Sedgewick: http://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
(but the original implementation did not work) http://read.seas.harvard.edu/~kohler/notes/llrb.html
python implementation: https://github.com/peterhil/leftrb
http://www.cs.princeton.edu/~rs/talks/LLRB/Java/RedBlackBST.java


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
