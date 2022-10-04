/* katalog/src/create-and-destroy.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "memory-manager.h"

const int defaultStringValueLength = 16;

/* BEGIN PROLOG_UNIVERSAL_TYPE */

static PROLOG_UNIVERSAL_TYPE * createUniversalStruct(
	int type,
	int integerValue,
	int maxNameLength,
	char * name,
	PROLOG_UNIVERSAL_TYPE * value1,
	PROLOG_UNIVERSAL_TYPE * value2,
	PROLOG_UNIVERSAL_TYPE * next
) {
	PROLOG_UNIVERSAL_TYPE * result = (PROLOG_UNIVERSAL_TYPE *)mmAlloc(sizeof(PROLOG_UNIVERSAL_TYPE));

	result->mark = 0;
	result->type = type;
	result->integerValue = integerValue;
	result->maxNameLength = maxNameLength;
	result->name = name;
	result->value1 = value1;
	result->value2 = value2;
	/* result->value3 = NULL; */
	result->next = next;
	/* result->aux = NULL; */

	addItemToMemMgrRecords(result);

	return result;
}

/* If name != NULL then copy it, and set maxNameLength = strlen(name) + 1 */
/* If name == NULL and maxNameLength > 1 then mmAlloc(maxNameLength * sizeof(char)) and zero-fill it */
/* If name == NULL and maxNameLength <= 0 then set maxNameLength = the default maxStringValueLength; then mmAlloc and zero-fill */

static PROLOG_UNIVERSAL_TYPE * allocateStringAndCreateUniversalStruct(
	int type,
	int integerValue,
	int maxNameLength,
	char * name,
	PROLOG_UNIVERSAL_TYPE * value1,
	PROLOG_UNIVERSAL_TYPE * value2,
	PROLOG_UNIVERSAL_TYPE * next
) {

	if (name != NULL) {
		const int len = strlen(name);

		if (maxNameLength <= len) {
			maxNameLength = len + 1;
		}
		/* This allows you to allocate a buffer longer than len + 1 chars if you wish */
	} else if (maxNameLength <= 0) {
		maxNameLength = defaultStringValueLength;
	}

	char * buf = (char *)mmAlloc(maxNameLength * sizeof(char));

	memset(buf, 0, maxNameLength * sizeof(char));

	if (name != NULL) {
		strcpy(buf, name);
	}

	return createUniversalStruct(type, integerValue, maxNameLength, buf, value1, value2, next);
}

void freeUniversalStruct(PROLOG_UNIVERSAL_TYPE * expr) {

	if (expr->name != NULL) {
		mmFree(expr->name);
		expr->name = NULL;
	}

	if (expr->value1 != NULL) {
		freeUniversalStruct(expr->value1);
		expr->value1 = NULL;
	}

	if (expr->value2 != NULL) {
		freeUniversalStruct(expr->value2);
		expr->value2 = NULL;
	}

	if (expr->next != NULL) {
		freeUniversalStruct(expr->next);
		expr->next = NULL;
	}

	mmFree(expr);
}

/* END PROLOG_UNIVERSAL_TYPE */

/* **** Value struct creation functions **** */

PROLOG_INTEGER * createInteger(int value) {
	return createUniversalStruct(
		prologType_Integer,
		value,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

PROLOG_EXPRESSION_LIST_ELEMENT * createExpressionListElement(PROLOG_EXPRESSION * expr, PROLOG_EXPRESSION_LIST_ELEMENT * next) {
	return createUniversalStruct(
		prologType_ExpressionListElement,
		0,
		0,
		NULL,
		expr,
		NULL,
		next
	);
}

PROLOG_VARIABLE * createVariable(char * name) {

	/* Ensure that name does not contain ( or ) */
	if (strchr(name, '(') != NULL || strchr(name, ')')) {
		fprintf(stderr, "createVariable() : The name '%s' contains an illegal character: '(' or ')'.\n", name);
		fatalError("createVariable() : String contains an illegal character");
	}

	return allocateStringAndCreateUniversalStruct(
		prologType_Variable,
		0,
		0,
		name,
		NULL,
		NULL,
		NULL
	);
}

PROLOG_NAME_LIST_ELEMENT * createNameListElement(char * name, PROLOG_NAME_LIST_ELEMENT * next) {
	return allocateStringAndCreateUniversalStruct(
		prologType_NameListElement,
		0,
		0,
		name,
		NULL,
		NULL,
		next
	);
}

PROLOG_NAME_VALUE_LIST_ELEMENT * createNameValueListElement(char * name, PROLOG_EXPRESSION * value, PROLOG_NAME_VALUE_LIST_ELEMENT * next) {
	failIf(name == NULL, "createNameValueListElement() : name == NULL");

	return allocateStringAndCreateUniversalStruct(
		prologType_NameValueListElement,
		0,
		0,
		name,
		value,
		NULL,
		next
	);
}

STRING_BUILDER_TYPE * createStringBuilder(int bufIncSize) {
	const int defaultBufferIncrementSize = 16;

	STRING_BUILDER_TYPE * result = createUniversalStruct(
		prologType_StringBuilder,
		(bufIncSize > 0) ? bufIncSize : defaultBufferIncrementSize,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	);

	/* failIf(getBufferSizeIncrementInStringBuilder(result) <= 0, "StringBuilder roundUpStringTypeBufferSize() : getBufferSizeIncrementInStringBuilder(result) <= 0"); */

	return result;
}

PROLOG_FUNCTOR * createFunctor(char * name, PROLOG_EXPRESSION_LIST_ELEMENT * args) {
	return allocateStringAndCreateUniversalStruct(
		prologType_Functor,
		0,
		0,
		name,
		args,
		NULL,
		NULL
	);
}

PROLOG_GOAL * createGoal(char * name, PROLOG_EXPRESSION_LIST_ELEMENT * args) {
	return allocateStringAndCreateUniversalStruct(
		prologType_Goal,
		0,
		0,
		name,
		args,
		NULL,
		NULL
	);
}

PROLOG_GOAL_LIST_ELEMENT * createGoalListElement(PROLOG_GOAL * goal, PROLOG_GOAL_LIST_ELEMENT * next) {
	return createUniversalStruct(
		prologType_GoalListElement,
		0,
		0,
		NULL,
		goal,
		NULL,
		next
	);
}

PROLOG_CLAUSE * createClause(PROLOG_GOAL * head, PROLOG_GOAL_LIST_ELEMENT * tail) {
	return createUniversalStruct(
		prologType_Clause,
		0,
		0,
		NULL,
		head,
		tail,
		NULL
	);
}

PROLOG_CLAUSE_LIST_ELEMENT * createClauseListElement(PROLOG_CLAUSE * clause, PROLOG_CLAUSE_LIST_ELEMENT * next) {
	return createUniversalStruct(
		prologType_ClauseListElement,
		0,
		0,
		NULL,
		clause,
		NULL,
		next
	);
}

PROLOG_UNIVERSAL_TYPE * createNull() {
	return createUniversalStruct(
		prologType_Null,
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

PROLOG_UNIVERSAL_TYPE * createCutReturn(int cutReturnNumber) {
	return createUniversalStruct(
		prologType_CutReturn,
		cutReturnNumber,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

BINARY_TREE_NODE_TYPE * createBinaryTreeNode(char * key, PROLOG_UNIVERSAL_TYPE * value, BINARY_TREE_NODE_TYPE * ltree, BINARY_TREE_NODE_TYPE * rtree) {
	return allocateStringAndCreateUniversalStruct(
		prologType_BinaryTreeNode,
		0,
		0,
		key,
		ltree,
		value,
		rtree
	);
}

/* **** The End **** */
