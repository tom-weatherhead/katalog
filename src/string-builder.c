/* katalog/src/string-builder.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#include "create-and-destroy.h"
#include "memory-manager.h"
#include "string-builder.h"

/* Functions */

static int roundUpStringTypeBufferSize(int n, int bufIncSize) {
	failIf(bufIncSize <= 0, "StringBuilder roundUpStringTypeBufferSize() : bufIncSize <= 0");

	return ((n + bufIncSize - 1) / bufIncSize) * bufIncSize;
}

BOOL isStringBuilderEmpty(STRING_BUILDER_TYPE * sb) {
	return sb == NULL || sb->name == NULL || sb->maxNameLength == 0 || strlen(sb->name) == 0;
}

/* TODO: Use this in parser.c : */

BOOL stringInBuilderIs(STRING_BUILDER_TYPE * sb, char * str) {
	return sb != NULL && str != NULL && sb->name != NULL && !strcmp(sb->name, str);
}

void clearStringBuilder(STRING_BUILDER_TYPE * sb) {

	if (sb != NULL && sb->name != NULL && sb->maxNameLength > 0) {
		memset(sb->name, 0, sb->maxNameLength * sizeof(char));
	}
}

/* newMinimumSize must already include one for the terminating null char */

static void ensureStringBuilderSize(STRING_BUILDER_TYPE * sb, int newMinimumSize) {
	newMinimumSize = roundUpStringTypeBufferSize(newMinimumSize, getBufferSizeIncrementInStringBuilder(sb));

	if (newMinimumSize > sb->maxNameLength) {
		const int numBytes = newMinimumSize * sizeof(char);
		char * newBuf = (char *)mmAlloc(numBytes);

		memset(newBuf, 0, numBytes);

		if (sb->name != NULL) {
			strcpy(newBuf, sb->name);
			mmFree(sb->name);
		}

		sb->name = newBuf;
		sb->maxNameLength = newMinimumSize;
	}
}

STRING_BUILDER_TYPE * appendToStringBuilder(STRING_BUILDER_TYPE * sb, char * strToAppend) {

	if (sb == NULL) {
		sb = createStringBuilder(0);
	}

	failIf(strToAppend == NULL, "appendToStringBuilder() : strToAppend == NULL");

	const int oldStrLen = (sb->name == NULL) ? 0 : strlen(sb->name);

	ensureStringBuilderSize(sb, oldStrLen + strlen(strToAppend) + 1);

	if (sb->name != NULL) {
		strcat(sb->name, strToAppend);
	}

	return sb;
}

STRING_BUILDER_TYPE * appendCharToStringBuilder(STRING_BUILDER_TYPE * sb, char c) {

	if (sb == NULL) {
		sb = createStringBuilder(0);
	}

	const int oldStrLen = (sb->name == NULL) ? 0 : strlen(sb->name);

	ensureStringBuilderSize(sb, oldStrLen + 2);

	failIf(sb->name == NULL, "appendCharToStringBuilder() : sb->name == NULL");

	sb->name[oldStrLen] = c;
	sb->name[oldStrLen + 1] = '\0';

	return sb;
}

STRING_BUILDER_TYPE * appendCharsToStringBuilder(STRING_BUILDER_TYPE * sb, char * src, int numChars) {

	if (sb == NULL) {
		sb = createStringBuilder(0);
	}

	failIf(src == NULL, "appendCharsToStringBuilder() : src == NULL");

	const int oldStrLen = (sb->name == NULL) ? 0 : strlen(sb->name);

	ensureStringBuilderSize(sb, oldStrLen + numChars + 1);

	failIf(sb->name == NULL, "appendCharToStringBuilder() : sb->name == NULL");

	memcpy(sb->name + oldStrLen, src, numChars * sizeof(char));
	sb->name[oldStrLen + numChars] = '\0';

	return sb;
}

/* **** The End **** */
