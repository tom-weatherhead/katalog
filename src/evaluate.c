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
#include "string-builder.h"
#include "substitution.h"
#include "variable.h"

static PROLOG_CLAUSE_LIST_ELEMENT * knowledgeBase = NULL;

/* STRING_BUILDER_TYPE * toString(PROLOG_UNIVERSAL_TYPE * this, STRING_BUILDER_TYPE * sb) {

	switch (this->type) {
		case prologType_Variable:
			return appendToStringBuilder(sb, this->name);

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("toString() : Bad object type");
			return NULL;
	}
} */

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

		/* case prologType_Substitution:
		case prologType_Functor:
		case prologType_Goal:
		case prologType_Clause: */
		default:
			fprintf(stderr, "equals() : Bad object type %d\n", this->type);
			fatalError("equals() : Bad object type");
			return FALSE;
	}
}

SET_OF_STRINGS * findBindingVariables(PROLOG_UNIVERSAL_TYPE * this) {

	if (this == NULL) {
		return NULL;
	}

	switch (this->type) {
		case prologType_Integer:
			return NULL;

		case prologType_Variable:

			if (isVariableNonBinding(this)) {
				return NULL;
			} else {
				return createNameListElement(this->name, NULL);
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
			return NULL;
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

	/* printf("applySubstitution() : this->type is %d\n", this->type); */

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
			// Do not use "if (this == otherExpr)", which just compares references.
			return createNull();
		} else if (that->type == prologType_Variable) {
			return unify(that, this);
		} else {
			return NULL; // The PrologIntegerLiteral and the IPrologExpression are not unifiable.
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
	knowledgeBase = createClauseListElement(clause, knowledgeBase);
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

static PROLOG_SUBSTITUTION * proveGoalListHelper(
	PROLOG_GOAL_LIST_ELEMENT * goalList,
	PROLOG_SUBSTITUTION * oldSubstitution,
	SET_OF_STRINGS * parentVariablesToAvoid
	/* , SET_OF_STRINGS * variablesInQuery */
) {
	failIf(goalList == NULL, "proveGoalList() : goalList == NULL");

	if (goalList->next != NULL) {
		printf("WARNING: proveGoalList() currently only handles goalLists of length one.\n");
	}

	PROLOG_CLAUSE_LIST_ELEMENT * ptr;

	for (ptr = knowledgeBase; ptr != NULL; ptr = ptr->next) {
		PROLOG_CLAUSE * clause = getClauseInClauseListElement(ptr);
		PROLOG_SUBSTITUTION * unifier = unify(getGoalInGoalListElement(goalList), getHeadInClause(clause));

		if (unifier != NULL) {
			printf("\nunifier: ");
			printExpression(unifier);
			printf("GoalInGoalList: ");
			printExpression(getGoalInGoalListElement(goalList));
			printf("HeadInClause: ");
			printExpression(getHeadInClause(clause));
			printf("Substituted GoalInGoalList: ");
			printExpression(applySubstitution(getGoalInGoalListElement(goalList), unifier));
			printf("Substituted HeadInClause: ");
			printExpression(applySubstitution(getHeadInClause(clause), unifier));

			return unifier;
		}
	}

	return NULL;
}

PROLOG_SUBSTITUTION * proveGoalList(PROLOG_GOAL_LIST_ELEMENT * goalList) {
	SET_OF_STRINGS * variablesInQuery = findBindingVariables(goalList);

	/* TODO: If a non-NULL unifier is returned, print the variables in the unifier
	that are also in variablesInQuery.
	This will avoid printing a ton of unneeded intermediate variables. */

	PROLOG_SUBSTITUTION * rawUnifier = proveGoalListHelper(goalList, createNull(), NULL);

	if (rawUnifier == NULL) {
		printf("proveGoalList() : No unifier found.\n");

		return NULL; /* No solution found. */
	}

	STRING_BUILDER_TYPE * sb = printExpressionToStringBuilder(NULL, rawUnifier);

	printf("proveGoalList() : The raw unifier is: %s\n", sb->name);

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

	sb = printExpressionToStringBuilder(NULL, filteredUnifier);

	printf("proveGoalList() : The filtered unifier is: %s\n", sb->name);

	return filteredUnifier;
}

/* **** The End **** */
