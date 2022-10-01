/* katalog/src/functor-expression.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "evaluate.h"
#include "string-builder.h"
#include "substitution.h"
#include "utilities.h"
#include "variable.h"

PROLOG_SUBSTITUTION * unifyFunctor(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that) {
	if (that->type == prologType_Variable) {
		return unify(that, this);
	}

	// A PrologFunctorExpression can unify with a PrologFunctorExpression;
	// a PrologGoal can unify with a PrologGoal,
	// but a PrologFunctorExpression cannot unify with a PrologGoal.

	if (
		that->type != prologType_Functor ||
		strcmp(this->name, that->name) ||
		getLinkedListLength(getArgumentsInFunctorOrGoal(this)) != getLinkedListLength(getArgumentsInFunctorOrGoal(that))
	) {
		return NULL;
	}

	PROLOG_SUBSTITUTION * substitution = createNull();
	PROLOG_EXPRESSION_LIST_ELEMENT * ptr1;
	PROLOG_EXPRESSION_LIST_ELEMENT * ptr2 = getArgumentsInFunctorOrGoal(that);

	for (ptr1 = getArgumentsInFunctorOrGoal(this); ptr1 != NULL; ptr1 = ptr1->next) {
		PROLOG_EXPRESSION * newExpr1 = applySubstitution(getExpressionInExpressionListElement(ptr1), substitution);
		PROLOG_EXPRESSION * newExpr2 = applySubstitution(getExpressionInExpressionListElement(ptr2), substitution);
		PROLOG_SUBSTITUTION * substitution2 = unify(newExpr1, newExpr2);

		if (substitution2 == NULL) {
			return NULL;
		}

		substitution = compose(substitution, substitution2);
		ptr2 = ptr2->next;
	}

	return substitution;
}

/* **** The End **** */
