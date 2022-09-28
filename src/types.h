/* katalog/src/types.h */

/* Preprocessor defines */

#if !defined(BOOL) && !defined(FALSE) && !defined(TRUE)
/* Our poor man's Boolean data type: */
#define BOOL int
#define FALSE 0
#define TRUE 1
#endif

/* void fail(char * str, char * file, int line);

#define failIf(b, str) if (b) { fail(str, __FILE__, __LINE__); }

#define fatalError(str) fail(str, __FILE__, __LINE__) */

/* **** The End **** */
