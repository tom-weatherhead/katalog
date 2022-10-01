/* katalog/src/variable.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "evaluate.h"
#include "string-builder.h"
#include "substitution.h"

/* The following supports non-binding variables such as _ and _Foo .
// See http://www.csupomona.edu/~jrfisher/www/prolog_tutorial/2_3.html

// This may contradict http://www.learnprolognow.org/lpnpage.php?pagetype=html&pageid=lpn-htmlse2 ,
// which implies that only _ is non-binding, and that any other variable that begins with _ is a normal, binding variable. */

BOOL isVariableNonBinding(PROLOG_UNIVERSAL_TYPE * this) {
	return this->type == prologType_Variable && this->name != NULL && strlen(this->name) > 0 && this->name[0] == '_';
}

PROLOG_SUBSTITUTION * unifyVariable(PROLOG_VARIABLE * this, PROLOG_EXPRESSION * otherExpr) {

	if (
		equals(this, otherExpr) ||
		isVariableNonBinding(this) ||
		/* 2014/03/13 : Don't add the binding { X = _ } to any substitution.
		// But what about a binding such as { X = foo(_) } ? */
		(otherExpr->type == prologType_Variable && isVariableNonBinding(otherExpr))
	) {
		return createNull(); /* I.e. createEmptySubstitution(); */
	} else if (containsVariable(otherExpr, this)) {
		/* This is the "occurs" check. */
		return NULL; /* This PrologVariable and the IPrologExpression are not unifiable. */
	} else {
		return createNameValueListElement(this->name, otherExpr, NULL);
	}
}

/* **** The End **** */
