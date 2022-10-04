/* katalog/src/set-of-strings.h */

/* See e.g. facility/src/string-set.h */

#define SET_OF_STRINGS PROLOG_UNIVERSAL_TYPE

BOOL setOfStringsContains(SET_OF_STRINGS * set, char * str);
SET_OF_STRINGS * addToSetOfStrings(char * str, SET_OF_STRINGS * set);
SET_OF_STRINGS * unionOfSetsOfStrings(SET_OF_STRINGS * set1, SET_OF_STRINGS * set2);

/* **** The End **** */
