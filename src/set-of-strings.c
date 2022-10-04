/* katalog/src/set-of-strings.c */

/* See e.g. facility/src/string-set.c */

/* TODO: Implement the set as a height-balanced (AVL) tree */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "set-of-strings.h"

BOOL setOfStringsContains(SET_OF_STRINGS * set, char * str) {

	for (; set != NULL; set = set->next) {

		if (!strcmp(str, set->name)) {
			return TRUE;
		}
	}

	return FALSE;
}

SET_OF_STRINGS * addToSetOfStrings(char * str, SET_OF_STRINGS * set) {

	if (!setOfStringsContains(set, str)) {
		set = createNameListElement(str, set);
	}

	return set;
}

SET_OF_STRINGS * unionOfSetsOfStrings(SET_OF_STRINGS * set1, SET_OF_STRINGS * set2) {
	SET_OF_STRINGS * result = NULL;
	SET_OF_STRINGS * ptr;

	/* First, clone set1 */

	for (ptr = set1; ptr != NULL; ptr = ptr->next) {
		result = createNameListElement(ptr->name, result);
	}

	for (ptr = set2; ptr != NULL; ptr = ptr->next) {
		result = addToSetOfStrings(ptr->name, result);
	}

	return result;
}

/* AVL tree in (ML?) */

/* This tree can be used as either a set of strings
or a dictionary with string-type keys and any type of values */

#define BINARY_TREE_NODE_TYPE PROLOG_UNIVERSAL_TYPE

static BOOL isKeyLessThan(char * key1, char * key2) {
	return strcmp(key1, key2) < 0;
}

static BINARY_TREE_NODE_TYPE * createBinaryTreeLeaf() {
	return NULL;
}

static BOOL isBinaryTreeLeaf(BINARY_TREE_NODE_TYPE * node) {
	return node == NULL;
}

/* static BINARY_TREE_NODE_TYPE * createBinaryTreeNode(char * key, BINARY_TREE_NODE_TYPE * ltree, BINARY_TREE_NODE_TYPE * rtree) {
	fatalError("createBinaryTreeNode() not implemented");
	return NULL;
}

static char * getKeyInBinaryTree(BINARY_TREE_NODE_TYPE * node) {
	fatalError("getKeyInBinaryTree() not implemented");
	return NULL;
}

static BINARY_TREE_NODE_TYPE * getLeftSubtree(BINARY_TREE_NODE_TYPE * node) {
	fatalError("getLeftSubtree() not implemented");
	return NULL;
}

static BINARY_TREE_NODE_TYPE * getRightSubtree(BINARY_TREE_NODE_TYPE * node) {
	fatalError("getRightSubtree() not implemented");
	return NULL;
} */

static int treeHeight(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return 0;
	}

	const int lheight = treeHeight(getLeftSubtree(node));
	const int rheight = treeHeight(getRightSubtree(node));

	return ((lheight > rheight) ? lheight : rheight) + 1;
}

/*   A            B
 *  / \          / \
 * a   B   ->   A   c
 *    / \      / \
 *   b   c    a   b
 *
fun RotateLeft Lf = Lf
	| RotateLeft (Br(_,_,Lf)) = Lf  (* Error *)
	| RotateLeft (Br((k1,_,x1),ltree,(Br((k2,_,x2),rltree,rrtree)))) =
		NewNode(k2,x2,NewNode(k1,x1,ltree,rltree),rrtree); */

static BINARY_TREE_NODE_TYPE * rotateLeft(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return createBinaryTreeLeaf();
	}

	BINARY_TREE_NODE_TYPE * rtree = getRightSubtree(node);

	failIf(isBinaryTreeLeaf(rtree), "rotateLeft() : RightSubtree is a leaf node");

	char * key1 = getKeyInBinaryTree(node);
	char * key2 = getKeyInBinaryTree(rtree);
	BINARY_TREE_NODE_TYPE * ltree = getLeftSubtree(node);
	BINARY_TREE_NODE_TYPE * rltree = getLeftSubtree(rtree);
	BINARY_TREE_NODE_TYPE * rrtree = getRightSubtree(rtree);

	BINARY_TREE_NODE_TYPE * newLtree = createBinaryTreeNode(key1, ltree, rltree);

	return createBinaryTreeNode(key2, newLtree, rrtree);
}

/*     B        A
 *    / \      / \
 *   A   c -> a   B
 *  / \          / \
 * a   b        b   c
 *
fun RotateRight Lf = Lf
	| RotateRight (Br(_,Lf,_)) = Lf  (* Error *)
	| RotateRight (Br((k1,_,x1),(Br((k2,_,x2),lltree,lrtree)),rtree)) =
		NewNode(k2,x2,lltree,NewNode(k1,x1,lrtree,rtree)); */

static BINARY_TREE_NODE_TYPE * rotateRight(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return createBinaryTreeLeaf();
	}

	BINARY_TREE_NODE_TYPE * ltree = getLeftSubtree(node);

	failIf(isBinaryTreeLeaf(ltree), "rotateRight() : LeftSubtree is a leaf node");

	char * key1 = getKeyInBinaryTree(node);
	char * key2 = getKeyInBinaryTree(ltree);
	BINARY_TREE_NODE_TYPE * rtree = getRightSubtree(node);
	BINARY_TREE_NODE_TYPE * lltree = getLeftSubtree(ltree);
	BINARY_TREE_NODE_TYPE * lrtree = getRightSubtree(ltree);

	BINARY_TREE_NODE_TYPE * newRtree = createBinaryTreeNode(key1, lrtree, rtree);

	return createBinaryTreeNode(key2, lltree, newRtree);
}

/* fun RotateRightLeft Lf = Lf
	| RotateRightLeft (Br((k,_,x),ltree,rtree)) =
		RotateLeft(NewNode(k,x,ltree,RotateRight(rtree))); */

static BINARY_TREE_NODE_TYPE * rotateRightLeft(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return createBinaryTreeLeaf();
	}

	char * key = getKeyInBinaryTree(node);
	BINARY_TREE_NODE_TYPE * ltree = getLeftSubtree(node);
	BINARY_TREE_NODE_TYPE * rtree = getRightSubtree(node);

	return rotateLeft(createBinaryTreeNode(key, ltree, rotateRight(rtree)));
}

/* fun RotateLeftRight Lf = Lf
	| RotateLeftRight (Br((k,_,x),ltree,rtree)) =
		RotateRight(NewNode(k,x,RotateLeft(ltree),rtree)); */

static BINARY_TREE_NODE_TYPE * rotateLeftRight(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return createBinaryTreeLeaf();
	}

	char * key = getKeyInBinaryTree(node);
	BINARY_TREE_NODE_TYPE * ltree = getLeftSubtree(node);
	BINARY_TREE_NODE_TYPE * rtree = getRightSubtree(node);

	return rotateRight(createBinaryTreeNode(key, rotateLeft(ltree), rtree));
}

/* In Br( (k, 1, x), Lf, Lf ), the 1 appears to be the subtree height.

fun AVLInsert (k, x, Lf) = Br( (k, 1, x), Lf, Lf )
  | AVLInsert (k, x, Br((k1, h1, x1), ltree, rtree)) =
		if Order.less(k,k1) then  (* x goes into ltree *)
      let
		    val newltree = AVLInsert(k,x,ltree)
				val newtree = NewNode(k1,x1,newltree,rtree)
      in
		    if TreeHeight(newltree) <= TreeHeight(rtree) + 1 then
				  newtree
        else if Order.less(k,NodeKey(newltree)) then
		      RotateRight(newtree)
				else
          RotateLeftRight(newtree)
		  end
    else if Order.less(k1,k) then
		  let
				val newrtree = AVLInsert(k,x,rtree)
        val newtree = NewNode(k1,x1,ltree,newrtree)
		  in
				if TreeHeight(newrtree) <= TreeHeight(ltree) + 1 then
          newtree
		    else if Order.less(NodeKey(newrtree),k) then
				  RotateLeft(newtree)
        else
		      RotateRightLeft(newtree)
      end
		else
			Br((k1, h1, x), ltree, rtree); (* Just replace the contents *) */

BINARY_TREE_NODE_TYPE * avlTreeInsert(char * key, BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return createBinaryTreeNode(key, createBinaryTreeLeaf(), createBinaryTreeLeaf());
	}

	char * key1 = getKeyInBinaryTree(node);
	BINARY_TREE_NODE_TYPE * ltree = getLeftSubtree(node);
	BINARY_TREE_NODE_TYPE * rtree = getRightSubtree(node);

	if (isKeyLessThan(key, key1)) {
		BINARY_TREE_NODE_TYPE * newltree = avlTreeInsert(key, ltree);
		BINARY_TREE_NODE_TYPE * newtree = createBinaryTreeNode(key1, newltree, rtree);

		if (treeHeight(newltree) <= treeHeight(rtree) + 1) {
			return newtree;
		} else if (isKeyLessThan(key, getKeyInBinaryTree(newltree))) {
			return rotateRight(newtree);
		} else {
			return rotateLeftRight(newtree);
		}
	} else if (isKeyLessThan(key1, key)) {
		BINARY_TREE_NODE_TYPE * newrtree = avlTreeInsert(key, rtree);
		BINARY_TREE_NODE_TYPE * newtree = createBinaryTreeNode(key1, ltree, newrtree);

		if (treeHeight(newrtree) <= treeHeight(ltree) + 1) {
			return newtree;
		} else if (isKeyLessThan(getKeyInBinaryTree(newrtree), key)) {
			return rotateLeft(newtree);
		} else {
			return rotateRightLeft(newtree);
		}
	} else {
		/* Just replace the contents */
		return createBinaryTreeNode(key1, ltree, rtree);
	}
}

/* fun SimpleBalance Lf = Lf
  | SimpleBalance (Br((k,h,x),ltree,rtree)) =
		let
      val tree = Br((k,h,x),ltree,rtree)
		  val lh = TreeHeight( ltree )
      val rh = TreeHeight( rtree )
		in
      if lh > rh + 1 then
		    RotateRight( tree )
      else if rh > lh + 1 then
		    RotateLeft( tree )
      else
		    tree
		end

fun AVLDelete (_, Lf) = Lf
  | AVLDelete (k, (Br((k1,h1,x1),ltree,rtree))) =
		if Order.less(k,k1) then
      SimpleBalance( NewNode( k1, x1, AVLDelete( k, ltree ), rtree ) )
		else if Order.less(k1,k) then
      SimpleBalance( NewNode( k1, x1, ltree, AVLDelete( k, rtree ) ) )
		else if IsEmpty(ltree) then
      rtree
		else if IsEmpty(rtree) then
      ltree
		else if TreeHeight(ltree) < TreeHeight(rtree) then
      AVLDelete( k, RotateLeft( Br((k1,h1,x1),ltree,rtree) ) )
		else
      AVLDelete( k, RotateRight( Br((k1,h1,x1),ltree,rtree) ) );

fun AVLVerifyTree( Lf ) = true
  | AVLVerifyTree( Br( (k, h, _), ltree, rtree ) ) =
		let
      val lh = TreeHeight( ltree )
		  val rh = TreeHeight( rtree )
    in
		  h >= lh + 1 andalso h >= rh + 1 andalso
      ( (h = lh + 1 andalso rh + 1 >= lh) orelse
		    (h = rh + 1 andalso lh + 1 >= rh) ) andalso
			( IsEmpty(ltree) orelse Order.less(NodeKey(ltree),k) ) andalso
			( IsEmpty(rtree) orelse Order.less(k,NodeKey(rtree)) ) andalso
      AVLVerifyTree( ltree ) andalso AVLVerifyTree( rtree )
		end
 */

void avlTreeInOrderTraversal(BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return;
	}

	avlTreeInOrderTraversal(getLeftSubtree(node));
	printf("Key: '%s'\n", getKeyInBinaryTree(node));
	avlTreeInOrderTraversal(getRightSubtree(node));
}

BOOL isKeyInAvlTree(char * key, BINARY_TREE_NODE_TYPE * node) {

	if (isBinaryTreeLeaf(node)) {
		return FALSE;
	}

	/* Binary search */
	const int comparison = strcmp(key, getKeyInBinaryTree(node));

	if (comparison < 0) {
		return isKeyInAvlTree(key, getLeftSubtree(node));
	} else if (comparison > 0) {
		return isKeyInAvlTree(key, getRightSubtree(node));
	} else {
		return TRUE;
	}
}

/* **** The End **** */
