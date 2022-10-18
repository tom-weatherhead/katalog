/* katalog/src/set-of-strings.h */

/* See e.g. facility/src/string-set.h */

#define SET_OF_STRINGS PROLOG_UNIVERSAL_TYPE

SET_OF_STRINGS * createSetOfStrings();

BOOL setOfStringsContains(SET_OF_STRINGS * set, char * str);

SET_OF_STRINGS * addToSetOfStrings(char * str, SET_OF_STRINGS * set);

/* No delete functionality is needed yet */

SET_OF_STRINGS * unionOfSetsOfStrings(SET_OF_STRINGS * set1, SET_OF_STRINGS * set2);

PROLOG_NAME_VALUE_LIST_ELEMENT * createNameValueListFromSetOfStrings(SET_OF_STRINGS * set, PROLOG_NAME_VALUE_LIST_ELEMENT * tail);

/* **** The End **** */
