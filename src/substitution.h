/* katalog/src/substitution.h */

BOOL substitutionContainsOnlyVariables(PROLOG_SUBSTITUTION * this);
BOOL substitutionContainsVariableName(PROLOG_SUBSTITUTION * this, char * name);
PROLOG_EXPRESSION * lookupValueInSubstitutionList(char * name, PROLOG_SUBSTITUTION * sub);
PROLOG_SUBSTITUTION * compose(PROLOG_SUBSTITUTION * this, PROLOG_SUBSTITUTION * otherSub);

/* **** The End **** */
