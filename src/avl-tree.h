/* katalog/src/avl-tree.h */

BINARY_TREE_NODE_TYPE * avlTreeInsert(char * key, BINARY_TREE_NODE_TYPE * node);
BOOL isKeyInAvlTree(char * key, BINARY_TREE_NODE_TYPE * node);
void avlTreeInOrderTraversal(BINARY_TREE_NODE_TYPE * node);

/* **** The End **** */
