/* katalog/src/set-of-strings.c */

/* See e.g. facility/src/string-set.c */

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

/* **** The End **** */
