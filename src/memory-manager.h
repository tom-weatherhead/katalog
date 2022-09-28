/* katalog/src/memory-manager.h */

void * mmAlloc(int numBytes);
void mmFree(void * ptr);
void mmPrintReport();

void addItemToMemMgrRecords(PROLOG_UNIVERSAL_TYPE * item);
int collectGarbage(PROLOG_UNIVERSAL_TYPE * exprTreesToMark[]);
int freeAllStructs();

/* **** The End **** */
