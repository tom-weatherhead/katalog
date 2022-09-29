/* katalog/src/parser.c */

/* **** Parsing (recursive descent - a real half-assed parser) **** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "char-source.h"
#include "string-builder.h"

/* #include "associative-array.h"
#include "evaluate.h"
#include "create-and-destroy.h"
#include "memory-manager.h"
#include "parser.h"
#include "utilities.h" */

/* Function prototypes */

/* Forward references */


/* Parse an expression */

/* static PROLOG_EXPRESSION * parseExpression(CharSource * cs) {
	if idStartingWithLowerCaseLetter then create functor (for now, don't allow parameters on functors)
	else if idStartingWithUpperCaseLetter then create variable
	else error
}

static PROLOG_EXPRESSION_LIST_ELEMENT * parseBracketedExpressionListTail(CharSource * cs) {
	if ")" then return NULL;

	consume ","

	expr = parseExpression(cs);
	next = parseBracketedExpressionListTail(cs);

	return createExpressionListElement(expr, next);
}

static PROLOG_EXPRESSION_LIST_ELEMENT * parseBracketedExpressionList(CharSource * cs) {

	if (!consume("(")) {
		return NULL;
	}

	expr = parseExpression(cs);
	next = parseBracketedExpressionListTail(cs);

	return createExpressionListElement(expr, next);
}

static PROLOG_EXPRESSION * parseFunctor(CharSource * cs) {
	goalName = getLowerCaseId(cs);
	argList = parseBracketedExpressionList(cs);

	return createFunctor(goalName, argList);
} */

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

/* static PROLOG_EXPRESSION * parseGoal(CharSource * cs) {
	goalName = getLowerCaseId(cs);
	argList = parseBracketedExpressionList(cs);

	return createGoal(goalName, argList);
}

static PROLOG_EXPRESSION * parseGoalListHelper(CharSource * cs) {

	if "." then return NULL;

	consume ","
	goal = parseGoal();
	next = parseGoalListHelper();

	return createGoalListElement(goal, next);
} */

/* The goals in the goal list are separated by commas. */
/* The list is terminated by a dot ('.'). */

/* static PROLOG_EXPRESSION * parseGoalList(CharSource * cs) {
	return NULL;
}

static PROLOG_EXPRESSION * parseClauseTail(CharSource * cs) {

	if "." then return NULL;

	consume ":-"
	goal = parseGoal();
	next = parseGoalListHelper();

	return createGoalListElement(goal, next);
} */

/* TODO here: Parse a goal */
/* Then parse either a dot or a clause tail */
/* A clause tail is :- followed by a non-empty goal list, concluded by a dot. */

static PROLOG_EXPRESSION * parseClause(char * firstIdentifier, CharSource * cs) {
	return NULL;
}

static PROLOG_EXPRESSION * parseQuery(CharSource * cs) {
	/* TODO here: Parse a goal (or a goal list?), concluded by a dot. */

	return NULL;
}

PROLOG_INPUT * parseInput(CharSource * cs) {
	STRING_BUILDER_TYPE * sb = getIdentifier(cs, NULL, NULL);

	if (isStringBuilderEmpty(sb)) {
		fatalError("parseInput() : Expected a line of input, found EOF");
		return NULL;
	}

	char * firstIdentifier = sb->name;

	if (!strcmp(firstIdentifier, "?-")) {
		return parseQuery(cs);
	} else {
		return parseClause(firstIdentifier, cs);
	}

	return NULL;
}

/* **** The End **** */
