/* katalog/src/tests.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "avl-tree.h"
#include "char-source.h"
#include "create-and-destroy.h"
#include "evaluate.h"
#include "memory-manager.h"
#include "parser.h"
#include "print.h"
#include "string-builder.h"

/* Functions */

static PROLOG_INPUT * processInput(char * str) {
	CharSource * cs = createCharSource(str);
	PROLOG_INPUT * parseTree = parseInput(cs);

	freeCharSource(cs);

	failIf(parseTree == NULL, "processInput() : parseTree == NULL");
	failIf(parseTree->type != prologType_Clause && parseTree->type != prologType_GoalListElement, "processInput() : parseTree is neither a clause nor a list of goals");

	STRING_BUILDER_TYPE * sb = printExpressionToStringBuilder(NULL, parseTree);

	/* printf("parseTree as string: '%s'\n", getStringInStringBuilder(sb)); */

	failIf(strcmp(getStringInStringBuilder(sb), str), "processInput: Parsed string differs from input.");

	if (parseTree->type == prologType_Clause) {
		printf("Adding clause to knowledge base...\n");
		addClauseToKnowledgeBase(parseTree);

		return parseTree;
	} else {
		return proveGoalList(parseTree);
	}
}

static void multitest(char * inputs[], char * expectedOutputs[]) {
	char * actualOutput = NULL;

	BOOL outputValuesMatch = TRUE;
	char * input = NULL;
	char * expectedOutput = NULL;
	int i;
	STRING_BUILDER_TYPE * sb = NULL;

	printf("\nclearKnowledgeBase()\n");
	clearKnowledgeBase();

	for (i = 0; outputValuesMatch; ++i) {
		input = inputs[i];
		expectedOutput = expectedOutputs[i];

		if (input == NULL || expectedOutput == NULL) {
			printf("\nmultitest: break\n");
			break;
		}

		printf("\nmultitest: input is '%s'\n", input);

		PROLOG_INPUT * value = processInput(input);

		actualOutput = NULL;

		if (value == NULL) {
			actualOutput = "No unifier found.";
		} else if (value->type == prologType_Clause) {
			actualOutput = "Clause added.";
		} else {
			failIf(value->type != prologType_Substitution && value->type != prologType_Null, "processInput() returned a value of an unexpected type");

			clearStringBuilder(sb);
			sb = printExpressionToStringBuilder(sb, value);

			actualOutput = sb->name;
		}

		outputValuesMatch = strlen(expectedOutput) == 0 || !strcmp(actualOutput, expectedOutput);
	}

	if (!outputValuesMatch) {
		fprintf(stderr, "\nTest failed:\n");
		fprintf(stderr, "  Input: %s\n", input);
		fprintf(stderr, "  Expected output: %s\n", expectedOutput);
		fprintf(stderr, "  Actual output: %s\n\n", actualOutput);
	}

	clearKnowledgeBase();
	freeAllStructs();

	if (!outputValuesMatch) {
		exit(1);
	}
}

static void randomizeAlphabet(char * buf) {
	int i;

	for (i = 0; i < 26; ++i) {
		buf[i] = i + 'A';
	}

	/* We assume that srand(time(NULL)); has already been called */

	for (i = 0; i < 26; ++i) {
		const int j = rand() % 26;
		const char c = buf[i];

		buf[i] = buf[j];
		buf[j] = c;
	}
}

static void avlTreeRandomizedAlphabetInsertDeleteTest() {
	BINARY_TREE_NODE_TYPE * tree = NULL;
	char buf[26];
	char key[2];
	int i;

	key[1] = '\0';

	printf("avlTree: Inserting randomized alphabet...\n");

	randomizeAlphabet(buf);

	for (i = 0; i < 26; ++i) {
		key[0] = buf[i];
		printf("  avlTree: Inserting key '%s'\n", key);
		tree = avlTreeInsertKey(key, tree);
	}

	printf("avlTree: Deleting randomized alphabet...\n");

	randomizeAlphabet(buf);

	for (i = 0; i < 26; ++i) {
		key[0] = buf[i];
		printf("  avlTree: Deleting key '%s'\n", key);
		tree = avlTreeDelete(key, tree);
	}
}

static void avlTreeTest() {
	BINARY_TREE_NODE_TYPE * tree = NULL;

	/* tree = avlTreeInsertKey("b", tree);
	tree = avlTreeInsertKey("j", tree);
	tree = avlTreeInsertKey("g", tree);
	tree = avlTreeInsertKey("a", tree);
	tree = avlTreeInsertKey("e", tree);
	tree = avlTreeInsertKey("f", tree);
	tree = avlTreeInsertKey("i", tree);
	tree = avlTreeInsertKey("c", tree);
	tree = avlTreeInsertKey("h", tree);
	tree = avlTreeInsertKey("d", tree); */

	tree = avlTreeInsertKeyAndValue("b", createInteger(2), tree);
	tree = avlTreeInsertKeyAndValue("j", createInteger(10), tree);
	tree = avlTreeInsertKeyAndValue("g", createInteger(7), tree);
	tree = avlTreeInsertKeyAndValue("a", createInteger(1), tree);
	tree = avlTreeInsertKeyAndValue("e", createInteger(5), tree);
	tree = avlTreeInsertKeyAndValue("f", createInteger(6), tree);
	tree = avlTreeInsertKeyAndValue("i", createInteger(9), tree);
	tree = avlTreeInsertKeyAndValue("c", createInteger(3), tree);
	tree = avlTreeInsertKeyAndValue("h", createInteger(8), tree);
	tree = avlTreeInsertKeyAndValue("d", createInteger(4), tree);

	avlTreeInOrderTraversal(tree);

	PROLOG_UNIVERSAL_TYPE * valueG = lookupValueInAvlTree("g", tree, NULL);

	printf("Lookup 'g' (1) -> ");

	if (valueG == NULL) {
		printf("NULL\n");
	} else {
		printExpression(valueG);
	}

	tree = avlTreeDelete("e", tree);
	tree = avlTreeDelete("q", tree);
	tree = avlTreeDelete("g", tree);

	valueG = lookupValueInAvlTree("g", tree, NULL);

	printf("Lookup 'g' (2) -> ");

	if (valueG == NULL) {
		printf("NULL\n");
	} else {
		printExpression(valueG);
	}

	avlTreeRandomizedAlphabetInsertDeleteTest();
}

void runTests() {
	printf("\nRunning tests...\n");

	/* Test of goals with no arguments */

	char * inputsGoalsWithNoArgs[] = {
		"b.", /* b is true */
		"a :- b.", /* If b then a */
		"?- a.", /* Query: Is a true? */
		NULL
	};
	char * expectedResultsGoalsWithNoArgs[] = {
		"",
		"",
		"[]", /* The unifier is the empty substitution */
		NULL
	};

	multitest(inputsGoalsWithNoArgs, expectedResultsGoalsWithNoArgs);

	/* List reversal test - functor notation */

	char * inputsListReversalFunctor[] = {
		"accRev(cons(H, T), A, R) :- accRev(T, cons(H, A), R).",
		"accRev(nil, A, A).",
		"?- accRev(nil, nil, R).",
		"?- accRev(cons(1, cons(2, nil)), nil, R).",
		"?- accRev(cons(1, cons(2, cons(3, nil))), nil, R).",
		NULL
	};
	char * expectedResultsListReversalFunctor[] = {
		"", /* ClauseAdded */
		"", /* ClauseAdded */
		"", /* Satisfied */
		"", /* Satisfied */
		"[R -> cons(3, cons(2, cons(1, nil)))]",
		NULL
	};

	multitest(inputsListReversalFunctor, expectedResultsListReversalFunctor);

	/* TODO: List reversal test - list notation */

	/* ['accRev([H | T], A, R):-  accRev(T, [H | A], R).', PrologGlobalInfo.ClauseAdded],
	['accRev([], A, A).', PrologGlobalInfo.ClauseAdded],
	['rev(L, R) :- accRev(L, [], R).', PrologGlobalInfo.ClauseAdded],
	['?- rev([1, 2, 3, 4], R).', success('R -> [4, 3, 2, 1]')],
	['?- accRev([1, 2, 3, 4], [], R).', ['Satisfied']],
	['?- accRev([], [], R).', ['Satisfied', '[R -> []]']],
	['?- accRev([1], [], R).', ['Satisfied', '[R -> [1]]']],
	['?- accRev([1, 2], [], R).', ['Satisfied', '[R -> [2, 1]]']],
	['?- accRev([1, 2, 3], [], R).', ['Satisfied', '[R -> [3, 2, 1]]']],
	['?- accRev([1, 2, 3, 4], [], R).', ['Satisfied', '[R -> [4, 3, 2, 1]]']] */

	/* Basic cut test */

	char * inputsBasicCut1[] = {
		"g(X) :- h(X), !, i(X).",
		"g(86).",
		"h(7).",
		"i(13).",
		"?- g(X).",
		NULL
	};
	char * expectedResultsBasicCut1[] = {
		"",
		"",
		"",
		"",
		"No unifier found.",
		NULL
	};

	multitest(inputsBasicCut1, expectedResultsBasicCut1);

	char * inputsBasicCut2[] = {
		"g(86).",
		"g(X) :- h(X), !, i(X).",
		"h(7).",
		"i(13).",
		"?- g(X).",
		NULL
	};
	char * expectedResultsBasicCut2[] = {
		"",
		"",
		"",
		"",
		"[X -> 86]",
		NULL
	};

	multitest(inputsBasicCut2, expectedResultsBasicCut2);

	char * inputsBasicCut3[] = {
		"g(X) :- h(X), i(X).",
		"g(86).",
		"h(7).",
		"i(13).",
		"?- g(X).",
		NULL
	};
	char * expectedResultsBasicCut3[] = {
		"",
		"",
		"",
		"",
		"[X -> 86]",
		NULL
	};

	multitest(inputsBasicCut3, expectedResultsBasicCut3);

	char * inputsBasicCut4[] = {
		"g(X) :- h(X), !, i(X).",
		"g(86).",
		"h(13).",
		"i(13).",
		"?- g(X).",
		NULL
	};
	char * expectedResultsBasicCut4[] = {
		"",
		"",
		"",
		"",
		"[X -> 13]",
		NULL
	};

	multitest(inputsBasicCut4, expectedResultsBasicCut4);

	avlTreeTest();

	printf("\nDone.\n");
}

/* test('LL(1) Prolog not test', () => {
	prologTest([
		['foo(X) :- \\+ bar(X).', PrologGlobalInfo.ClauseAdded],
		['bar(7).', PrologGlobalInfo.ClauseAdded],
		['?- foo(7).', [PrologGlobalInfo.NotSatisfied]],
		['?- foo(13).', [PrologGlobalInfo.Satisfied]]
	]);
});

test('LL(1) Prolog Italian crossword test', () => {
	prologTest([
		['word(astante,  a,s,t,a,n,t,e).', PrologGlobalInfo.ClauseAdded],
		['word(astoria,  a,s,t,o,r,i,a).', PrologGlobalInfo.ClauseAdded],
		['word(baratto,  b,a,r,a,t,t,o).', PrologGlobalInfo.ClauseAdded],
		['word(cobalto,  c,o,b,a,l,t,o).', PrologGlobalInfo.ClauseAdded],
		['word(pistola,  p,i,s,t,o,l,a).', PrologGlobalInfo.ClauseAdded],
		['word(statale,  s,t,a,t,a,l,e).', PrologGlobalInfo.ClauseAdded],
		['member(X,[X|_]).', PrologGlobalInfo.ClauseAdded],
		['member(X,[_|L]) :- member(X,L).', PrologGlobalInfo.ClauseAdded],
		['unique_list([]).', PrologGlobalInfo.ClauseAdded],
		['unique_list([X|L]) :- \\+ member(X, L), unique_list(L).', PrologGlobalInfo.ClauseAdded],
		[
			'crossword(V1, V2, V3, H1, H2, H3) :- word(V1, _, V12, _, V14, _, V16, _), word(V2, _, V22, _, V24, _, V26, _), word(V3, _, V32, _, V34, _, V36, _), word(H1, _, V12, _, V22, _, V32, _), word(H2, _, V14, _, V24, _, V34, _), word(H3, _, V16, _, V26, _, V36, _), unique_list([V1, V2, V3, H1, H2, H3]).',
			PrologGlobalInfo.ClauseAdded
		],
		[
			'?- crossword(V1, V2, V3, H1, H2, H3).',
			[
				'Satisfying substitution is: [H1 -> astoria; H2 -> baratto; H3 -> statale; V1 -> astante; V2 -> cobalto; V3 -> pistola]',
				'Satisfied'
			]
		]
	]);
});

test('LL(1) Prolog permutation test 1', () => {
	prologTest(
		[
			['append([], L, L).', PrologGlobalInfo.ClauseAdded],
			['append([X | Y], L, [X | Z]) :- append(Y, L, Z).', PrologGlobalInfo.ClauseAdded],
			['permutation([], []).', PrologGlobalInfo.ClauseAdded],
			[
				'permutation(L, [H|T]) :- append(V, [H|U], L), append(V, U, W), permutation(W, T).',
				PrologGlobalInfo.ClauseAdded
			],
			[
				'?- permutation([red, green, blue], C).',
				'Satisfying substitution is: [C -> [red, green, blue]]\n' +
					'Satisfying substitution is: [C -> [red, blue, green]]\n' +
					'Satisfying substitution is: [C -> [green, red, blue]]\n' +
					'Satisfying substitution is: [C -> [green, blue, red]]\n' +
					'Satisfying substitution is: [C -> [blue, red, green]]\n' +
					'Satisfying substitution is: [C -> [blue, green, red]]\n' +
					'Number of solutions found: 6\n' +
					PrologGlobalInfo.Satisfied +
					'\n'
			]
		],
		{ findAllSolutions: true }
	);
});
*/

/* **** The End **** */
