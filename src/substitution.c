/* katalog/src/substitution.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "evaluate.h"
#include "string-builder.h"
#include "substitution.h"
#include "variable.h"

/* public get length(): number {
	return this.SubstitutionList.size;
}

public toString(): string {
	const entries = [...this.SubstitutionList.entries()];

	// e1[0] is e1.key; e1[1] would be e1.value
	// entries.sort((e1: [string, IPrologExpression], e2: [string, IPrologExpression]) => e1[0].localeCompare(e2[0]));
	entries.sort((e1, e2) => e1[0].localeCompare(e2[0]));

	return `[${entries.map(([key, value]) => `${key} -> ${value}`).join('; ')}]`;
} */

BOOL substitutionContainsOnlyVariables(PROLOG_SUBSTITUTION * this) {
	PROLOG_SUBSTITUTION * ptr;

	for (ptr = this; ptr != NULL; ptr = ptr->next) {

		if (ptr->type != prologType_Variable) {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL substitutionContainsVariableName(PROLOG_SUBSTITUTION * this, char * name) {
	PROLOG_SUBSTITUTION * ptr;

	for (ptr = this; ptr != NULL; ptr = ptr->next) {

		if (!strcmp(ptr->name, name)) {
			return TRUE;
		}
	}

	return FALSE;
}

PROLOG_EXPRESSION * lookupValueInSubstitutionList(char * name, PROLOG_SUBSTITUTION * sub) {
	PROLOG_SUBSTITUTION * ptr;

	for (ptr = sub; ptr != NULL; ptr = ptr->next) {
		failIf(ptr->type != prologType_NameValueListElement, "lookupValueInSubstitutionList() : ptr->type != prologType_NameValueListElement");
		failIf(ptr->name == NULL, "lookupValueInSubstitutionList() : ptr->name == NULL");

		if (!strcmp(ptr->name, name)) {
			return getValueInNameValueListElement(ptr);
		}
	}

	return NULL;
}

PROLOG_SUBSTITUTION * compose(PROLOG_SUBSTITUTION * this, PROLOG_SUBSTITUTION * otherSub) {

	if (this->type == prologType_Null) {
		return otherSub;
	} else if (otherSub->type == prologType_Null) {
		return this;
	}

	PROLOG_SUBSTITUTION * newSub = NULL;
	PROLOG_SUBSTITUTION * ptr;

	/* 1) Apply the otherSub substitution to this's terms. */

	/* for (const [key, sub] of this.SubstitutionList.entries()) {
		const newUnifiable = sub.ApplySubstitution(otherSub) as IPrologExpression;

		if (typeof newUnifiable === 'undefined') {
			throw new Error(
				'PrologSubstitution.Compose() : The result of applying a substitution to an IUnifiable is not an IUnifiable.'
			);
		}

		newSub.SubstitutionList.set(key, newUnifiable);
	} */

	for (ptr = this; ptr != NULL; ptr = ptr->next) {
		PROLOG_EXPRESSION * newUnifiable = applySubstitution(getValueInNameValueListElement(ptr), otherSub);

		failIf(newUnifiable == NULL, "PrologSubstitution.Compose() : The result of applying a substitution to an IUnifiable is not an IUnifiable.");

		newSub = createNameValueListElement(ptr->name, newUnifiable, newSub);
	}

	/* 2) Remove identities. */
	/* const varsToRemove: string[] = [];

	for (const [key, value] of newSub.SubstitutionList.entries()) {
		if (isIPrologVariable(value) && value.Name === key) {
			throw new Error(
				'PrologSubstitution: An identity should have been removed from the substitution, but was not.'
			);
			// varsToRemove.push(key);
		}

		// if (v.equals(newSub.SubstitutionList.get(key))) {
		if (typeof value === 'string' && value === key) {
			varsToRemove.push(key);
		}
	}

	for (const v of varsToRemove) {
		newSub.SubstitutionList.delete(v);
	} */

	PROLOG_SUBSTITUTION ** pp = &newSub;

	for (ptr = newSub; ptr != NULL;) {
		PROLOG_EXPRESSION * value = getValueInNameValueListElement(ptr);

		if (value->type == prologType_Variable && !strcmp(ptr->name, value->name)) {
			/* Remove the identity */
			*pp = ptr->next;
		} else {
			pp = &ptr->next;
		}

		ptr = *pp;
	}

	/* 3) Remove duplicate variables; i.e. add substitutions from keys in otherSub that are not keys in the "this" Substitution. */

	/* for (const [key, v] of otherSub.SubstitutionList.entries()) {
		//if (!newSub.SubstitutionList.ContainsKey(key))    // In error.
		if (!this.SubstitutionList.has(key)) {
			// Correct, according to the CS 486 course notes.
			newSub.SubstitutionList.set(key, v);
		}
	} */

	for (ptr = otherSub; ptr != NULL; ptr = ptr->next) {

		if (!substitutionContainsVariableName(this, ptr->name)) {
			newSub = createNameValueListElement(ptr->name, getValueInNameValueListElement(ptr), newSub);
		}
	}

	/* #if SUBSTITUTION_COMPOSITION_VERIFICATION */

	/* According to Kamin, we should ensure that no member of newSub.SubstitutionList.Keys appears in newSub.SubstitutionList.Values . */

	/* const variablesInValues = createSet<string>();

	for (const value of newSub.SubstitutionList.values()) {
		// variablesInValues.unionInPlace(value.FindBindingVariables());

		const bindingVariables = value.FindBindingVariables();

		for (const bv of bindingVariables) {
			variablesInValues.add(bv.Name);
		}

		// variablesInValues.unionInPlace(bindingVariables);
	}

	for (const key of newSub.SubstitutionList.keys()) {
		// if (variablesInValues.contains(new PrologVariable(key))) {
		if (variablesInValues.contains(key)) {
			// #if CONSOLE_WRITELINE
			// console.log(
			// 	'PrologSubstitution.Compose() : Unacceptable substitution; returning null.'
			// );
			// #endif
			throw new Error(
				`Unacceptable substitution; key == ${key}; this == ${this}; otherSub == ${otherSub}; newSub = ${newSub}`
			);
		}
	}
	// #endif */

	return newSub;
}

// public FindBindingVariables(): Set<PrologVariable> {
// 	const result = new Set<PrologVariable>();

// 	for (const key of this.SubstitutionList.keys()) {
// 		result.add(new PrologVariable(key));

// 		const v = this.SubstitutionList.get(key);

// 		if (typeof v !== 'undefined') {
// 			result.unionInPlace(v.FindBindingVariables());
// 		}
// 	}

// 	return result;
// }

// public get IsOneToOne(): boolean {
// 	const values: PrologVariable[] = [];

// 	for (const value of this.SubstitutionList.values()) {
// 		const vv = value as PrologVariable;

// 		if (typeof vv === 'undefined') {
// 			return false;
// 		}

// 		values.push(vv);
// 	}

// 	return values.length === Array.from(this.SubstitutionList.keys()).length;
// }

/* **** The End **** */
