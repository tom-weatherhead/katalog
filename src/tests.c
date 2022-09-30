/* katalog/src/tests.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "char-source.h"
#include "create-and-destroy.h"
#include "evaluate.h"
#include "memory-manager.h"
#include "parser.h"

static PROLOG_CLAUSE_LIST_ELEMENT * knowledgeBase = NULL;

/* Functions */

PROLOG_INPUT * processInput(char * str) {
	CharSource * cs = createCharSource(str);
	PROLOG_INPUT * parseTree = parseInput(cs);

	freeCharSource(cs);

	failIf(parseTree == NULL, "processInput() : parseTree == NULL");
	failIf(parseTree->type != prologType_Clause && parseTree->type != prologType_GoalListElement, "processInput() : parseTree is neither a clause nor a list of goals");

	if (parseTree->type == prologType_Clause) {
		printf("Adding clause to knowledge base...\n");
		knowledgeBase = createClauseListElement(parseTree, knowledgeBase);
	} else {
		printf("TODO: Executing query...\n");

		/* PROLOG_SUBSTITUTION * unifier = */ proveGoalList(parseTree);
	}

	return parseTree;
}

static void multitest(char * inputs[], char * expectedOutputs[]) {
	char * actualOutput = NULL;

	BOOL outputValuesMatch = TRUE;
	char * input = NULL;
	char * expectedOutput = NULL;
	int i;
	/* STRING_BUILDER_TYPE * sb = NULL; */

	knowledgeBase = NULL;

	for (i = 0; outputValuesMatch; ++i) {
		input = inputs[i];
		expectedOutput = expectedOutputs[i];

		if (input == NULL || expectedOutput == NULL) {
			printf("multitest: break\n");
			break;
		}

		printf("multitest: input is '%s'\n", input);

		/* PROLOG_INPUT * value = */ processInput(input);

		/* clearStringBuilder(sb);
		sb = printValueToStringEx(sb, value, NULL, TRUE);

		actualOutput = sb->name;

		outputValuesMatch = strlen(expectedOutput) == 0 || !strcmp(actualOutput, expectedOutput); */
	}

	if (!outputValuesMatch) {
		fprintf(stderr, "\nTest failed:\n");
		fprintf(stderr, "  Input: %s\n", input);
		fprintf(stderr, "  Expected output: %s\n", expectedOutput);
		fprintf(stderr, "  Actual output: %s\n\n", actualOutput);
	}

	/* knowledgeBase = NULL;
	freeAllStructs(); */

	if (!outputValuesMatch) {
		exit(1);
	}
}


void runTests() {
	printf("\nRunning tests...\n");

	char * inputsGoalsWithNoArgs[] = {
		"b.", /* b is true */
		"a :- b.", /* If b then a */
		"?- a.", /* Query: Is a true? */
		NULL
	};
	char * expectedResultsGoalsWithNoArgs[] = {
		"",
		"",
		"",
		NULL
	};

	multitest(inputsGoalsWithNoArgs, expectedResultsGoalsWithNoArgs);

	printf("\nDone.\n");
}

/*
test('LL(1) Prolog list reverse test', () => {
	prologTest([
		['accRev(cons(H, T), A, R) :- accRev(T, cons(H, A), R).', PrologGlobalInfo.ClauseAdded],
		['accRev(nil, A, A).', PrologGlobalInfo.ClauseAdded],
		['?- accRev(nil, nil, R).', ['Satisfied']],
		['?- accRev(cons(1, cons(2, nil)), nil, R).', ['Satisfied']],
		[
			'?- accRev(cons(1, cons(2, cons(3, nil))), nil, R).',
			['Satisfying substitution is: [R -> cons(3, cons(2, cons(1, nil)))]', 'Satisfied']
		],
		[
			'?- accRev(cons(1, cons(2, cons(3, cons(4, nil)))), nil, R).',
			[
				'Satisfying substitution is: [R -> cons(4, cons(3, cons(2, cons(1, nil))))]',
				'Satisfied'
			]
		]
	]);
});

test('LL(1) Prolog list reversal test', () => {
	prologTest([
		['accRev([H | T], A, R):-  accRev(T, [H | A], R).', PrologGlobalInfo.ClauseAdded],
		['accRev([], A, A).', PrologGlobalInfo.ClauseAdded],
		['rev(L, R) :- accRev(L, [], R).', PrologGlobalInfo.ClauseAdded],
		['?- rev([1, 2, 3, 4], R).', success('R -> [4, 3, 2, 1]')],
		['?- accRev([1, 2, 3, 4], [], R).', ['Satisfied']],
		['?- accRev([], [], R).', ['Satisfied', '[R -> []]']],
		['?- accRev([1], [], R).', ['Satisfied', '[R -> [1]]']],
		['?- accRev([1, 2], [], R).', ['Satisfied', '[R -> [2, 1]]']],
		['?- accRev([1, 2, 3], [], R).', ['Satisfied', '[R -> [3, 2, 1]]']],
		['?- accRev([1, 2, 3, 4], [], R).', ['Satisfied', '[R -> [4, 3, 2, 1]]']]
	]);
});

test('LL(1) Prolog not test', () => {
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
