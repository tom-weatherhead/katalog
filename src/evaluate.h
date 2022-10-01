/* katalog/src/evaluate.h */

BOOL equals(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that);
BOOL containsVariable(PROLOG_UNIVERSAL_TYPE * this, PROLOG_VARIABLE * v);
PROLOG_EXPRESSION * applySubstitution(PROLOG_UNIVERSAL_TYPE * this, PROLOG_SUBSTITUTION * sub);
PROLOG_SUBSTITUTION * unify(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that);
void clearKnowledgeBase();
void addClauseToKnowledgeBase(PROLOG_CLAUSE * clause);
PROLOG_SUBSTITUTION * proveGoalList(PROLOG_GOAL_LIST_ELEMENT * goalList);

/* **** The End **** */
