/* katalog/src/set-of-strings.c */

/* See e.g. facility/src/string-set.c */

/* TODO: Implement the set as a height-balanced (AVL) tree */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "avl-tree.h"
#include "create-and-destroy.h"
#include "set-of-strings.h"

SET_OF_STRINGS * createSetOfStrings() {
	return NULL;
}

BOOL setOfStringsContains(SET_OF_STRINGS * set, char * str) {

#if 1
	return isKeyInAvlTree(str, set);
#else
	for (; set != NULL; set = set->next) {

		if (!strcmp(str, set->name)) {
			return TRUE;
		}
	}

	return FALSE;
#endif
}

SET_OF_STRINGS * addToSetOfStrings(char * str, SET_OF_STRINGS * set) {

#if 1
	return avlTreeInsertKey(str, set);
#else
	if (!setOfStringsContains(set, str)) {
		set = createNameListElement(str, set);
	}

	return set;
#endif
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

#if 1
	return addAllMembersToOtherSet(set1, addAllMembersToOtherSet(set2, createSetOfStrings()));
#else
	SET_OF_STRINGS * result = createSetOfStrings();
	SET_OF_STRINGS * ptr;

	/* First, clone set1 */

	for (ptr = set1; ptr != NULL; ptr = ptr->next) {
		/* result = createNameListElement(ptr->name, result); */
		result = addToSetOfStrings(ptr->name, result);
	}

	for (ptr = set2; ptr != NULL; ptr = ptr->next) {
		result = addToSetOfStrings(ptr->name, result);
	}

	return result;
#endif
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
