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

/*   A            B
 *  / \          / \
 * a   B   ->   A   c
 *    / \      / \
 *   b   c    a   b
 *
fun RotateLeft Lf = Lf
	| RotateLeft (Br(_,_,Lf)) = Lf  (* Error *)
	| RotateLeft (Br((k1,_,x1),ltree,(Br((k2,_,x2),rltree,rrtree)))) =
		NewNode(k2,x2,NewNode(k1,x1,ltree,rltree),rrtree);

 *     B        A
 *    / \      / \
 *   A   c -> a   B
 *  / \          / \
 * a   b        b   c
 *
fun RotateRight Lf = Lf
	| RotateRight (Br(_,Lf,_)) = Lf  (* Error *)
	| RotateRight (Br((k1,_,x1),(Br((k2,_,x2),lltree,lrtree)),rtree)) =
		NewNode(k2,x2,lltree,NewNode(k1,x1,lrtree,rtree));

fun RotateRightLeft Lf = Lf
	| RotateRightLeft (Br((k,_,x),ltree,rtree)) =
		RotateLeft(NewNode(k,x,ltree,RotateRight(rtree)));

fun RotateLeftRight Lf = Lf
	| RotateLeftRight (Br((k,_,x),ltree,rtree)) =
		RotateRight(NewNode(k,x,RotateLeft(ltree),rtree));

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
			Br((k1, h1, x), ltree, rtree); (* Just replace the contents *)

fun SimpleBalance Lf = Lf
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

/* **** The End **** */
