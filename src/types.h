/* katalog/src/types.h */

/* Preprocessor defines */

#if !defined(BOOL) && !defined(FALSE) && !defined(TRUE)
/* Our poor man's Boolean data type: */
#define BOOL int
#define FALSE 0
#define TRUE 1
#endif

/* Type definitions */

typedef struct PROLOG_UNIVERSAL_STRUCT {
	/* Contains ten members. */

	int mark; /* All dynamically allocated structs must have this member */

	int type;
	/* TODO: Use a union? */
	int integerValue;

	int maxNameLength; /* Size (in chars) of the allocated buffer to which name points */
	char * name; /* Or use the char name[1]; trick at the end of the struct? */

	struct PROLOG_UNIVERSAL_STRUCT * value1;
	struct PROLOG_UNIVERSAL_STRUCT * value2;
	struct PROLOG_UNIVERSAL_STRUCT * value3;

	struct PROLOG_UNIVERSAL_STRUCT * next; /* To allow linked lists */

	void * aux;
} PROLOG_UNIVERSAL_TYPE;

#define PROLOG_CLAUSE PROLOG_UNIVERSAL_TYPE
#define PROLOG_CLAUSE_LIST_ELEMENT PROLOG_UNIVERSAL_TYPE
#define PROLOG_EXPRESSION PROLOG_UNIVERSAL_TYPE
#define PROLOG_EXPRESSION_LIST_ELEMENT PROLOG_UNIVERSAL_TYPE
#define PROLOG_FUNCTOR PROLOG_UNIVERSAL_TYPE
#define PROLOG_GOAL PROLOG_UNIVERSAL_TYPE
#define PROLOG_GOAL_LIST_ELEMENT PROLOG_UNIVERSAL_TYPE
#define PROLOG_INPUT PROLOG_UNIVERSAL_TYPE
#define PROLOG_NAME_LIST_ELEMENT PROLOG_UNIVERSAL_TYPE
#define PROLOG_NAME_VALUE_LIST_ELEMENT PROLOG_UNIVERSAL_TYPE
#define PROLOG_VARIABLE PROLOG_UNIVERSAL_TYPE
#define STRING_BUILDER_TYPE PROLOG_UNIVERSAL_TYPE

/* Substitution = associative array? */
#define PROLOG_SUBSTITUTION PROLOG_NAME_VALUE_LIST_ELEMENT

#define getValueInNameValueListElement(nvle) ((nvle)->value1)

#define getBufferSizeIncrementInStringBuilder(sb) ((sb)->integerValue)
#define getStringInStringBuilder(sb) ((sb)->name)

enum {
	prologType_Variable,
	prologType_Substitution,
	prologType_Functor,
	prologType_Goal,
	prologType_Clause,
	/* prologType_, */
	prologType_StringBuilder,

	prologType_ClauseListElement,
	prologType_ExpressionListElement,
	prologType_GoalListElement, /* I.e. Set<Variable> */
	prologType_NameListElement, /* I.e. Set<Variable> */
	prologType_NameValueListElement = prologType_Substitution
};

void fail(char * str, char * file, int line);

#define failIf(b, str) if (b) { fail(str, __FILE__, __LINE__); }

#define fatalError(str) fail(str, __FILE__, __LINE__)

/* **** The End **** */
