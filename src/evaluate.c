/* katalog/src/evaluate.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "string-builder.h"
#include "substitution.h"
#include "variable.h"

STRING_BUILDER_TYPE * toString(PROLOG_UNIVERSAL_TYPE * this, STRING_BUILDER_TYPE * sb) {

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
}

BOOL equals(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that) {

	switch (this->type) {
		case prologType_Variable:
			return that->type == prologType_Variable && !strcmp(this->name, that->name);

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("equals() : Bad object type");
			return FALSE;
	}
}

PROLOG_NAME_LIST_ELEMENT * findBindingVariables(PROLOG_UNIVERSAL_TYPE * this) {

	switch (this->type) {
		case prologType_Variable:

			if (isVariableNonBinding(this)) {
				return NULL;
			} else {
				return createNameListElement(this->name, NULL);
			}

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("findBindingVariables() : Bad object type");
			return NULL;
	}
}

BOOL containsVariable(PROLOG_UNIVERSAL_TYPE * this, PROLOG_VARIABLE * v) {

	switch (this->type) {
		case prologType_Variable:
			return equals(this, v);

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("containsVariable() : Bad object type");
			return FALSE;
	}
}

PROLOG_EXPRESSION * applySubstitution(PROLOG_UNIVERSAL_TYPE * this, PROLOG_SUBSTITUTION * sub) {
	PROLOG_UNIVERSAL_TYPE * value = NULL;

	switch (this->type) {
		case prologType_Variable:
			value = lookupValueInSubstitutionList(this->name, sub);

			if (value != NULL) {
				return value;
			}

			return this;

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("applySubstitution() : Bad object type");
			return NULL;
	}
}

PROLOG_SUBSTITUTION * unify(PROLOG_UNIVERSAL_TYPE * this, PROLOG_UNIVERSAL_TYPE * that) {

	switch (this->type) {
		case prologType_Variable:
			return unifyVariable(this, that);

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("unify() : Bad object type");
			return NULL;
	}
}

BOOL isGround(PROLOG_UNIVERSAL_TYPE * this) {

	switch (this->type) {
		case prologType_Variable:
			return FALSE;

		case prologType_Substitution:
		case prologType_Goal:
		case prologType_Clause:
		default:
			fatalError("isGround() : Bad object type");
			return FALSE;
	}
}

/* **** The End **** */
