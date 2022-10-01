/* katalog/src/print.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "types.h"

#include "char-source.h"
#include "create-and-destroy.h"
#include "string-builder.h"
#include "utilities.h"

/* Function prototypes */

/* Forward references */

/* Functions */

STRING_BUILDER_TYPE * printExpressionToStringBuilder(STRING_BUILDER_TYPE * sb, PROLOG_EXPRESSION * expr) {
	failIf(expr == NULL, "printExpressionToStringBuilder() : expr == NULL");

	PROLOG_UNIVERSAL_TYPE * ptr = NULL;
	char * separatorStr = "";
	const int intSprintfBufferLength = 16;
	char intSprintfBuffer[intSprintfBufferLength];

	if (sb == NULL) {
		sb = createStringBuilder(0);
	}

	switch (expr->type) {
		case prologType_Clause:
			sb = printExpressionToStringBuilder(sb, getHeadInClause(expr));
			ptr = getTailInClause(expr);

			if (ptr != NULL) {
				sb = appendToStringBuilder(sb, " :- ");

				while (ptr != NULL) {
					sb = appendToStringBuilder(sb, separatorStr);
					sb = printExpressionToStringBuilder(sb, ptr->value1); /* getGoalInGoalList */
					separatorStr = ", ";
					ptr = ptr->next;
				}
			}

			sb = appendToStringBuilder(sb, ".");
			break;

		case prologType_Functor:
		case prologType_Goal:
			sb = appendToStringBuilder(sb, getNameInValue(expr));
			ptr = getArgumentsInFunctorOrGoal(expr);

			if (ptr != NULL) {
				sb = appendToStringBuilder(sb, "(");

				while (ptr != NULL) {
					sb = appendToStringBuilder(sb, separatorStr);
					sb = printExpressionToStringBuilder(sb, ptr->value1); /* getExpressionInExpressionList */
					separatorStr = ", ";
					ptr = ptr->next;
				}

				sb = appendToStringBuilder(sb, ")");
			}

			break;

		case prologType_Integer:
			memset(intSprintfBuffer, 0, intSprintfBufferLength * sizeof(char));
			sprintf(intSprintfBuffer, "%d", getIntegerValueInValue(expr));
			sb = appendToStringBuilder(sb, intSprintfBuffer);
			break;

		case prologType_Null:
		case prologType_Substitution:
			sb = appendToStringBuilder(sb, "[");

			for (ptr = expr; ptr != NULL && ptr->type != prologType_Null; ptr = ptr->next) {
				sb = appendToStringBuilder(sb, separatorStr);
				sb = appendToStringBuilder(sb, ptr->name);
				sb = appendToStringBuilder(sb, " -> ");
				sb = printExpressionToStringBuilder(sb, getValueInNameValueListElement(ptr));
				separatorStr = "; ";
			}

			sb = appendToStringBuilder(sb, "]");
			break;

		case prologType_Variable:
			sb = appendToStringBuilder(sb, getNameInValue(expr));
			break;

		case prologType_GoalListElement:
			sb = appendToStringBuilder(sb, "?- "); /* This should be appended only if this GoalListElement is the start of a query */

			ptr = getTailInClause(expr);

			for (ptr = expr; ptr != NULL; ptr = ptr->next) {
				sb = appendToStringBuilder(sb, separatorStr);
				sb = printExpressionToStringBuilder(sb, getGoalInGoalListElement(ptr));
				separatorStr = ", ";
			}

			sb = appendToStringBuilder(sb, ".");
			break;

		default:
			fprintf(stderr, "printExpressionToStringBuilder() : Unexpected expr type %d\n", expr->type);
			fatalError("printExpressionToStringBuilder() : Unexpected expr type");
			return NULL;
	}

	return sb;
}

void printExpression(PROLOG_EXPRESSION * expr) {
	STRING_BUILDER_TYPE * sb = printExpressionToStringBuilder(NULL, expr);

	printf("%s\n", sb->name);
}

/* **** The End **** */
