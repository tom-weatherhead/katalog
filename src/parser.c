/* katalog/src/parser.c */

/* **** Parsing (recursive descent - a real half-assed parser) **** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "types.h"

#include "char-source.h"
#include "create-and-destroy.h"
#include "string-builder.h"
#include "utilities.h"

/* #include "associative-array.h"
#include "evaluate.h"
#include "memory-manager.h"
#include "parser.h" */

/* Function prototypes */

/* Forward references */

static PROLOG_FUNCTOR * parseFunctor(CharSource * cs);

/* A goal is: id ( goal-arg-list ) */
/* id must begin with a lower-case letter */
/* goal-arg-list either is empty (i.e. followed by a ')')
or is: goal-arg goal-arg-list-tail */
/* goal-arg-list-tail either is empty (i.e. followed by a ')')
or is: , goal-arg goal-arg-list-tail */
/* goal-arg is an "expression", which is one of:
- a functor? (which must begin with a lower-case letter, and may be followed by a bracketed list of parameters? (Each of these parameters would be an "expression"))
- a variable (which must begin with an upper-case letter)
- a numeric literal?
- a list literal? (e.g. [] or [1, 2, 3] or [HEAD | TAIL])
Syntactic sugar:
[HEAD | TAIL] is equivalent to cons(HEAD, TAIL)
[] equivalent to nil
cons and nil are parameterless functors

Goals and functors are syntactically identical, but are distinguished from each other based on context:

- A goal is always at the top level of a clause or query;
- A functor is used as a parameter to a goal or to a functor
*/

static PROLOG_EXPRESSION * parseExpression(CharSource * cs) {
	const int rewindPoint = cs->i;
	int dstBufAsInt = 0;
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (isStringBuilderEmpty(sb)) {
		fatalError("parseExpression() : Unexpected EOF");
		return NULL;
	} else if (isupper(sb->name[0])) {
		/* A variable */
		return createVariable(sb->name);
	} else if (safeAtoi(sb->name, &dstBufAsInt)) {
		return createInteger(dstBufAsInt);
	} else if (!islower(sb->name[0])) {
		fprintf(stderr, "sb->name is '%s'\n", sb->name);
		fatalError("parseExpression() : Expected a variable or a functor");
		return NULL;
	}

	cs->i = rewindPoint;

	return parseFunctor(cs);
}

static PROLOG_EXPRESSION_LIST_ELEMENT * parseBracketedExpressionListHelper(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (isStringBuilderEmpty(sb)) {
		fatalError("parseBracketedExpressionListHelper() : Unexpected EOF");
		return NULL;
	} else if (!strcmp(sb->name, ")")) {
		/* End of list */
		return NULL;
	} else if (strcmp(sb->name, ",")) {
		fatalError("parseBracketedExpressionListHelper() : Expected ,");
		return NULL;
	}

	PROLOG_EXPRESSION * expr = parseExpression(cs);
	PROLOG_EXPRESSION_LIST_ELEMENT * next = parseBracketedExpressionListHelper(cs);

	return createExpressionListElement(expr, next);
}

static PROLOG_EXPRESSION_LIST_ELEMENT * parseBracketedExpressionList(CharSource * cs) {
	const int rewindPoint = cs->i;
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (isStringBuilderEmpty(sb)) {
		fatalError("parseBracketedExpressionList() : Unexpected EOF");
		return NULL;
	} else if (strcmp(sb->name, "(")) {
		/* The goal or functor has no arguments */
		cs->i = rewindPoint;
		return NULL;
	}

	const int rewindPoint2 = cs->i;

	sb = getIdentifier(cs, NULL);

	if (!strcmp(sb->name, ")")) {
		/* The goal or functor has no arguments */
		return NULL;
	}

	cs->i = rewindPoint2;

	PROLOG_EXPRESSION * expr = parseExpression(cs);
	PROLOG_EXPRESSION_LIST_ELEMENT * next = parseBracketedExpressionListHelper(cs);

	return createExpressionListElement(expr, next);
}

/* parseFunctor and parseGoal are essentially identical except for their calls to createFunctor / createGoal */

static PROLOG_FUNCTOR * parseFunctor(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (strlen(sb->name) == 0) {
		fatalError("parseFunctor() : Identifier is the empty string");
		return NULL;
	} else if (!islower(sb->name[0])) {
		fatalError("parseFunctor() : Identifier does not begin with a lower-case letter");
		return NULL;
	}

	char * functorName = sb->name;
	/* printf("parseFunctor() : functorName is '%s'\n", functorName); */
	PROLOG_EXPRESSION_LIST_ELEMENT * argList = parseBracketedExpressionList(cs);

	return createFunctor(functorName, argList);
}

static PROLOG_GOAL * parseGoal(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (strlen(sb->name) == 0) {
		fatalError("parseGoal() : Identifier is the empty string");
		return NULL;
	} else if (!islower(sb->name[0])) {
		fatalError("parseGoal() : Identifier does not begin with a lower-case letter");
		return NULL;
	}

	char * goalName = sb->name;
	/* printf("parseGoal() : goalName is '%s'\n", goalName); */
	PROLOG_EXPRESSION_LIST_ELEMENT * argList = parseBracketedExpressionList(cs);

	return createGoal(goalName, argList);
}

static PROLOG_GOAL_LIST_ELEMENT * parseGoalListHelper(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (!strcmp(sb->name, ".")) {
		return NULL; /* End of goal list */
	} else if (strcmp(sb->name, ",")) {
		fatalError("parseGoalListHelper() : Expected . or ,");
		return NULL;
	}

	PROLOG_GOAL * goal = parseGoal(cs);
	PROLOG_GOAL_LIST_ELEMENT * next = parseGoalListHelper(cs);

	return createGoalListElement(goal, next);
}

/* The goals in the goal list are separated by commas. */
/* The list is terminated by a dot ('.'). */

static PROLOG_EXPRESSION * parseGoalList(CharSource * cs) {
	PROLOG_GOAL * goal = parseGoal(cs);
	PROLOG_GOAL_LIST_ELEMENT * next = parseGoalListHelper(cs);

	return createGoalListElement(goal, next);
}

static PROLOG_EXPRESSION * parseClauseTail(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (!strcmp(sb->name, ".")) {
		return NULL; /* The clause has no tail */
	} else if (strcmp(sb->name, ":-")) {
		fatalError("parseClauseTail() : Expected . or :-");
		return NULL;
	}

	PROLOG_GOAL * goal = parseGoal(cs);
	PROLOG_GOAL_LIST_ELEMENT * next = parseGoalListHelper(cs);

	return createGoalListElement(goal, next);
}

/* TODO here: Parse a goal */
/* Then parse either a dot or a clause tail */
/* A clause tail is :- followed by a non-empty goal list, concluded by a dot. */

static PROLOG_EXPRESSION * parseClause(CharSource * cs) {
	PROLOG_GOAL * head = parseGoal(cs);
	PROLOG_GOAL_LIST_ELEMENT * tail = parseClauseTail(cs);

	return createClause(head, tail);
}

static PROLOG_GOAL_LIST_ELEMENT * parseQuery(CharSource * cs) {
	/* TODO here: Parse a goal (or a goal list?), concluded by a dot. */

	return parseGoalList(cs);
}

PROLOG_INPUT * parseInput(CharSource * cs) {
	const int rewindPoint = cs->i;
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL);

	if (isStringBuilderEmpty(sb)) {
		fatalError("parseInput() : Expected a line of input, found EOF");
		return NULL;
	}

	if (!strcmp(sb->name, "?-")) {
		return parseQuery(cs);
	} else {
		cs->i = rewindPoint; /* Rewind the CharSource to un-read the identifier we just read */
		return parseClause(cs);
	}
}

/* **** The End **** */
