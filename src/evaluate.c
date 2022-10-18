/* katalog/src/evaluate.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "functor-expression.h"
#include "goal.h"
#include "print.h"
#include "set-of-strings.h"
#include "substitution.h"
#include "variable.h"

static PROLOG_CLAUSE_LIST_ELEMENT * knowledgeBase = NULL;
static int nextCutReturnNum = 0;
static int nextUnusedVariableNum = 0;

BOOL equals(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that) {
	failIf(this == NULL, "equals() : this == NULL");
	failIf(that == NULL, "equals() : that == NULL");

	if (this->type != that->type) {
		return FALSE;
	}

	switch (this->type) {
		case prologType_Integer:
			return getIntegerValueInValue(this) == getIntegerValueInValue(that);

		case prologType_Variable:
			return !strcmp(this->name, that->name);

		default:
			fprintf(stderr, "equals() : Bad object type %d\n", this->type);
			fatalError("equals() : Bad object type");
			return FALSE;
	}
}

SET_OF_STRINGS * findBindingVariables(PROLOG_UNIVERSAL_TYPE * this) {
	SET_OF_STRINGS * result = createSetOfStrings();

	if (this == NULL) {
		return result;
	}

	switch (this->type) {
		case prologType_Integer:
			return result;

		case prologType_Variable:

			if (isVariableNonBinding(this)) {
				return result;
			} else {
				/* return createNameListElement(this->name, NULL); */
				return addToSetOfStrings(this->name, result);
			}

		case prologType_Functor:
		case prologType_Goal:
			return findBindingVariables(getArgumentsInFunctorOrGoal(this));

		case prologType_Clause:
			return unionOfSetsOfStrings(
				findBindingVariables(getHeadInClause(this)),
				findBindingVariables(getTailInClause(this))
			);

		case prologType_ExpressionListElement:
			return unionOfSetsOfStrings(
				findBindingVariables(getExpressionInExpressionListElement(this)),
				findBindingVariables(this->next)
			);

		case prologType_GoalListElement:
			return unionOfSetsOfStrings(
				findBindingVariables(getGoalInGoalListElement(this)),
				findBindingVariables(this->next)
			);

		case prologType_Substitution:
		default:
			fprintf(stderr, "findBindingVariables() : Bad object type %d\n", this->type);
			fatalError("findBindingVariables() : Bad object type");
			return result;
	}
}

BOOL containsVariable(PROLOG_UNIVERSAL_TYPE * this, PROLOG_VARIABLE * v) {

	if (this == NULL) {
		return FALSE;
	}

	switch (this->type) {
		case prologType_Integer:
			return FALSE;

		case prologType_Variable:
			return equals(this, v);

		case prologType_Functor:
		case prologType_Goal:
			return containsVariable(getArgumentsInFunctorOrGoal(this), v);

		case prologType_Clause:
			return containsVariable(getHeadInClause(this), v) || containsVariable(getTailInClause(this), v);

		case prologType_ExpressionListElement:
			return containsVariable(getExpressionInExpressionListElement(this), v) || containsVariable(this->next, v);

		case prologType_GoalListElement:
			return containsVariable(getGoalInGoalListElement(this), v) || containsVariable(this->next, v);

		case prologType_Substitution:
		default:
			fprintf(stderr, "containsVariable() : Bad object type %d\n", this->type);
			fatalError("containsVariable() : Bad object type");
			return FALSE;
	}
}

PROLOG_EXPRESSION * applySubstitution(PROLOG_UNIVERSAL_TYPE * this, PROLOG_SUBSTITUTION * sub) {

	if (this == NULL || sub->type == prologType_Null) {
		return this;
	}

	failIf(sub->type != prologType_NameValueListElement, "applySubstitution() : sub->type != prologType_NameValueListElement");

	PROLOG_UNIVERSAL_TYPE * value = NULL;

	switch (this->type) {
		case prologType_Integer:
			return this;

		case prologType_Variable:
			value = lookupValueInSubstitutionList(this->name, sub);

			if (value != NULL) {
				return value;
			}

			return this;

		case prologType_Functor:
			return createFunctor(this->name, applySubstitution(getArgumentsInFunctorOrGoal(this), sub));

		case prologType_Goal:
			return createGoal(this->name, applySubstitution(getArgumentsInFunctorOrGoal(this), sub));

		case prologType_Clause:
			return createClause(
				applySubstitution(getHeadInClause(this), sub),
				applySubstitution(getTailInClause(this), sub)
			);

		case prologType_ExpressionListElement:
			return createExpressionListElement(
				applySubstitution(getExpressionInExpressionListElement(this), sub),
				applySubstitution(this->next, sub)
			);

		case prologType_GoalListElement:
			return createGoalListElement(
				applySubstitution(getGoalInGoalListElement(this), sub),
				applySubstitution(this->next, sub)
			);

		case prologType_Substitution:
		default:
			fprintf(stderr, "applySubstitution() : Bad object type %d\n", this->type);
			fatalError("applySubstitution() : Bad object type");
			return NULL;
	}
}

/* We need to distinguish between "not unifiable" and
"unifiable via the empty substitution"
-> NULL vs. prologType_Null
*/

PROLOG_SUBSTITUTION * unify(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that) {

	switch (this->type) {
		case prologType_Integer:

		if (equals(this, that)) {
			return createNull();
		} else if (that->type == prologType_Variable) {
			return unify(that, this);
		} else {
			return NULL; /* Not unifiable. */
		}

		case prologType_Variable:
			return unifyVariable(this, that);

		case prologType_Functor:
			return unifyFunctor(this, that);

		case prologType_Goal:
			return unifyGoal(this, that);

		case prologType_Substitution:
		case prologType_Clause:
		default:
			fprintf(stderr, "unify() : Bad object type %d\n", this->type);
			fatalError("unify() : Bad object type");
			return NULL;
	}
}

BOOL isGround(PROLOG_UNIVERSAL_TYPE * this) {

	switch (this->type) {
		case prologType_Integer:
			return TRUE;

		case prologType_Variable:
			return FALSE;

		case prologType_Substitution:
		case prologType_Functor:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fprintf(stderr, "isGround() : Bad object type %d\n", this->type);
			fatalError("isGround() : Bad object type");
			return FALSE;
	}
}

/*
C# :

private ProveGoalList(
	goalList: PrologGoal[],
	cutDetectorList: CutDetector[],
	goalNum: number,
	oldSubstitution: ISubstitution,
	parentVariablesToAvoid: IImmutableSet<IPrologVariable>,
	variablesInQuery: IPrologVariable[], // Print these variables and their values automatically upon success if there is no print() goal at the end
	listOfCurrentModules: PrologModule[]
): ISubstitution | undefined {
	if (goalNum >= goalList.length) {
		// The goal list has been satisfied.

		// console.log(
		// 	`ProveGoalList() : The goal list of length ${goalList.length} has been satisfied`
		// );
		// console.log(
		// 	'ProveGoalList() : Found solution:',
		// 	oldSubstitution.toString()
		// );
		// this.printDirect(
		// 	`ProveGoalList() : Found solution: ${oldSubstitution}`
		// );

		// **** Begin automatic printing ****
		// const lastGoal =
		// 	goalList.length > 0 ? goalList[goalList.length - 1] : undefined;

		// if (
		// 	typeof lastGoal !== 'undefined' &&
		// 	lastGoal.Name !== 'print'
		// ) {
		// 	// Don't do automatic printing if the last goal was a print() goal.
		this.AutomaticPrint(variablesInQuery, oldSubstitution);
		this.numSolutionsFound++;
		// }

		// **** End automatic printing ****

		// To continue searching for other solutions (i.e. if we are operating in "all" mode rather than "first" mode), return null.

		if (this.allMode) {
			return undefined;
		}

		return oldSubstitution;
	}

	...
}
*/

void clearKnowledgeBase() {
	knowledgeBase = NULL;
}

void addClauseToKnowledgeBase(PROLOG_CLAUSE * clause) {
	/* The order of this list is important.
	So append the new clause to the end of the linked list. */

	PROLOG_CLAUSE_LIST_ELEMENT ** pp = &knowledgeBase;

	while (*pp != NULL) {
		pp = &((*pp)->next);
	}

	*pp = createClauseListElement(clause, NULL);
}

/* Params to the C# proveGoalList() :
	goalList: PrologGoal[],
	cutDetectorList: CutDetector[],
	goalNum: number,
	oldSubstitution: ISubstitution,
	parentVariablesToAvoid: IImmutableSet<IPrologVariable>,
	variablesInQuery: IPrologVariable[], // Print these variables and their values automatically upon success if there is no print() goal at the end
	listOfCurrentModules: PrologModule[]
*/

static PROLOG_VARIABLE * createUnusedVariable() {
	char buf[20];

	sprintf(buf, "*var%d", nextUnusedVariableNum++);

	return createVariable(buf);
}

static PROLOG_CLAUSE * renameVariablesInClause(PROLOG_CLAUSE * clause) {
	/* SET_OF_STRINGS * bindingVariables = findBindingVariables(clause);
	PROLOG_SUBSTITUTION * sub = createNull(); */

	/* TODO: Iterate over the strings in the set */

	/* for (; bindingVariables != NULL; bindingVariables = bindingVariables->next) {
		sub = createNameValueListElement(bindingVariables->name, createUnusedVariable(), sub);
	} */

	/* PROLOG_SUBSTITUTION * sub = createNameValueListFromSetOfStrings(findBindingVariables(clause), createNull()); */
	PROLOG_SUBSTITUTION * sub = createNameValueListFromSetOfStrings(findBindingVariables(clause), NULL);
	PROLOG_SUBSTITUTION * ptr;

	if (sub == NULL) {
		return clause;
	}

	for (ptr = sub; ptr != NULL /* && ptr->type != prologType_Null */; ptr = ptr->next) {
		getValueInNameValueListElement(ptr) = createUnusedVariable();
	}

	return applySubstitution(clause, sub);
}

static PROLOG_GOAL_LIST_ELEMENT * copyGoalListAndAppend(PROLOG_GOAL_LIST_ELEMENT * goalList, PROLOG_GOAL_LIST_ELEMENT * goalListToAppend, int * pnCutReturnNumber) {

	if (goalList == NULL) {
		return goalListToAppend;
	}

	PROLOG_GOAL * goal = getGoalInGoalListElement(goalList);

	if (isCut(goal)) {
		const int cutReturnNumber = ++nextCutReturnNum;

		*pnCutReturnNumber = cutReturnNumber;
		getCutReturnNumberInCutReturnOrGoal(goal) = cutReturnNumber;
	}

	return createGoalListElement(goal, copyGoalListAndAppend(goalList->next, goalListToAppend, pnCutReturnNumber));
}

static PROLOG_SUBSTITUTION * proveGoalListHelper(
	PROLOG_GOAL_LIST_ELEMENT * goalList,
	PROLOG_SUBSTITUTION * oldSubstitution /* ,
	SET_OF_STRINGS * parentVariablesToAvoid
	, SET_OF_STRINGS * variablesInQuery */
) {

	if (goalList == NULL) {
		return oldSubstitution; /* The goal list has been proven. */
	}

	PROLOG_GOAL * goal = getGoalInGoalListElement(goalList);

	if (isCut(goal)) {
		/* The 'cut' goal always succeeeds trivially; it only affects backtracking */
		PROLOG_SUBSTITUTION * result = proveGoalListHelper(goalList->next, oldSubstitution);

		if (result == NULL) {
			/* **** Here is where the 'cut' magic happens **** */
			/* We are trying to backtrack through a cut. */
			const int cutReturnNumber = getCutReturnNumberInCutReturnOrGoal(goal);

			failIf(cutReturnNumber <= 0, "proveGoalListHelper() : Backtracking through a cut : cutReturnNumber in cut goal <= 0");
			/* printf("**** Cut number %d has been used ****\n", cutReturnNumber); */
			result = createCutReturn(cutReturnNumber);
		}

		return result;
	}

	goal = applySubstitution(goal, oldSubstitution);

	PROLOG_CLAUSE_LIST_ELEMENT * ptr;

	for (ptr = knowledgeBase; ptr != NULL; ptr = ptr->next) {
		PROLOG_CLAUSE * clause = getClauseInClauseListElement(ptr);
		PROLOG_CLAUSE * renamedClause = renameVariablesInClause(clause);

		PROLOG_SUBSTITUTION * unifier = unify(goal, getHeadInClause(renamedClause));

		if (unifier == NULL) {
			continue;
		}

		printf("\nGoalInGoalList: ");
		printExpression(getGoalInGoalListElement(goalList));
		printf("HeadInClause: ");
		printExpression(getHeadInClause(renamedClause));
		printf("unifier: ");
		printExpression(unifier);

		PROLOG_SUBSTITUTION * newSubstitution = compose(oldSubstitution, unifier);

		/* Create a goal list that is a copy of getTailInClause(renamedClause)
		with goalList->next appended to the end of it. */

		int cutReturnNumber = -1;
		PROLOG_GOAL_LIST_ELEMENT * newGoalList = copyGoalListAndAppend(getTailInClause(renamedClause), goalList->next, &cutReturnNumber);

		PROLOG_SUBSTITUTION * result = proveGoalListHelper(newGoalList, newSubstitution);

		if (result != NULL) {

			if (result->type == prologType_CutReturn && getCutReturnNumberInCutReturnOrGoal(result) == cutReturnNumber) {
				/* We have finished handling a cut that was backtracked through
				Now allow backtracking to resume */
				break; /* So that NULL will be returned. */
			}

			return result;
		}
	}

	return NULL;
}

PROLOG_SUBSTITUTION * proveGoalList(PROLOG_GOAL_LIST_ELEMENT * goalList) {
	SET_OF_STRINGS * variablesInQuery = findBindingVariables(goalList);

	/* TODO: If a non-NULL unifier is returned, print the variables in the unifier
	that are also in variablesInQuery.
	This will avoid printing a ton of unneeded intermediate variables. */

	PROLOG_SUBSTITUTION * rawUnifier = proveGoalListHelper(goalList, createNull());

	if (rawUnifier == NULL) {
		/* printf("proveGoalList() : No unifier found.\n"); */

		return NULL; /* No solution found. */
	}

	/* STRING_BUILDER_TYPE * sb = printExpressionToStringBuilder(NULL, rawUnifier);

	printf("proveGoalList() : The raw unifier is: %s\n", sb->name); */

	PROLOG_SUBSTITUTION * filteredUnifier = NULL;
	PROLOG_SUBSTITUTION * ptr;

	for (ptr = rawUnifier; ptr != NULL; ptr = ptr->next) {

		if (setOfStringsContains(variablesInQuery, ptr->name)) {
			filteredUnifier = createNameValueListElement(ptr->name, getValueInNameValueListElement(ptr), filteredUnifier);
		}
	}

	if (filteredUnifier == NULL) {
		filteredUnifier = createNull();
	}

	/* sb = printExpressionToStringBuilder(NULL, filteredUnifier);

	printf("proveGoalList() : The filtered unifier is: %s\n", sb->name); */

	return filteredUnifier;
}

/* **** The End **** */
