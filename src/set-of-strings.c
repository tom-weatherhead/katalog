/* katalog/src/set-of-strings.c */

/* See e.g. facility/src/string-set.c */

#include <stdlib.h>

#include "types.h"

#include "avl-tree.h"
#include "create-and-destroy.h"
#include "set-of-strings.h"

SET_OF_STRINGS * createSetOfStrings() {
	return NULL;
}

BOOL setOfStringsContains(SET_OF_STRINGS * set, char * str) {
	return isKeyInAvlTree(str, set);
}

SET_OF_STRINGS * addToSetOfStrings(char * str, SET_OF_STRINGS * set) {
	return avlTreeInsertKey(str, set);
}

static SET_OF_STRINGS * addAllMembersToOtherSet(SET_OF_STRINGS * set, SET_OF_STRINGS * otherSet) {

	if (set == NULL) {
		return otherSet;
	}

	otherSet = addAllMembersToOtherSet(getRightSubtree(set), otherSet);
	otherSet = addToSetOfStrings(set->name, otherSet);

	return addAllMembersToOtherSet(getLeftSubtree(set), otherSet);
}

SET_OF_STRINGS * unionOfSetsOfStrings(SET_OF_STRINGS * set1, SET_OF_STRINGS * set2) {
	/* This function does not need to know anything about
	how SET_OF_STRINGS is implemented (e.g. linked list vs. AVL tree) */

	return addAllMembersToOtherSet(set1, addAllMembersToOtherSet(set2, createSetOfStrings()));
}

PROLOG_NAME_VALUE_LIST_ELEMENT * createNameValueListFromSetOfStrings(SET_OF_STRINGS * set, PROLOG_NAME_VALUE_LIST_ELEMENT * tail) {

	if (set == NULL) { /* If the set is empty */
		return tail;
	}

	return createNameValueListFromSetOfStrings(getLeftSubtree(set),
		createNameValueListElement(set->name, NULL,
			createNameValueListFromSetOfStrings(getRightSubtree(set), tail)
		)
	);
}

/* **** The End **** */
