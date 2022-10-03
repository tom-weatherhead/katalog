/* katalog/src/create-and-destroy.h */

/* Create values */

PROLOG_CLAUSE * createClause(PROLOG_GOAL * head, PROLOG_GOAL_LIST_ELEMENT * tail);
PROLOG_CLAUSE_LIST_ELEMENT * createClauseListElement(PROLOG_CLAUSE * clause, PROLOG_CLAUSE_LIST_ELEMENT * next);
PROLOG_UNIVERSAL_TYPE * createCutReturn(int cutReturnNumber);
PROLOG_EXPRESSION_LIST_ELEMENT * createExpressionListElement(PROLOG_EXPRESSION * expr, PROLOG_EXPRESSION_LIST_ELEMENT * next);
PROLOG_FUNCTOR * createFunctor(char * name, PROLOG_EXPRESSION_LIST_ELEMENT * args);
PROLOG_GOAL * createGoal(char * name, PROLOG_EXPRESSION_LIST_ELEMENT * args);
PROLOG_GOAL_LIST_ELEMENT * createGoalListElement(PROLOG_GOAL * goal, PROLOG_GOAL_LIST_ELEMENT * next);
PROLOG_INTEGER * createInteger(int value);
PROLOG_NAME_LIST_ELEMENT * createNameListElement(char * name, PROLOG_NAME_LIST_ELEMENT * next);
PROLOG_NAME_VALUE_LIST_ELEMENT * createNameValueListElement(char * name, PROLOG_EXPRESSION * value, PROLOG_NAME_VALUE_LIST_ELEMENT * next);
PROLOG_UNIVERSAL_TYPE * createNull();
STRING_BUILDER_TYPE * createStringBuilder(int bufIncSize);
PROLOG_VARIABLE * createVariable(char * name);

/* Free stuff */

void freeUniversalStruct(PROLOG_UNIVERSAL_TYPE * expr);

/* **** The End **** */
