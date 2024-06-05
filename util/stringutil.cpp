
#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <vector>

namespace fdxx {

// https://github.com/alliedmodders/sourcemod/blob/master/core/logic/stringutil.cpp
char *ReplaceString(char *subject, size_t maxLen, const char *search, size_t searchLen, const char *replace, size_t replaceLen, bool caseSensitive)
{
	char *ptr = subject;
	size_t browsed = 0;
	size_t textLen = strlen(subject);

	/* It's not possible to search or replace */
	if (searchLen > textLen)
	{
		return NULL;
	}

	/* Handle the case of one byte replacement.
	 * It's only valid in one case.
	 */
	if (maxLen == 1)
	{
		/* If the search matches and the replace length is 0,
		 * we can just terminate the string and be done.
		 */
		if ((caseSensitive ? strcmp(subject, search) : strcasecmp(subject, search)) == 0 && replaceLen == 0)
		{
			*subject = '\0';
			return subject;
		}
		else
		{
			return NULL;
		}
	}

	/* Subtract one off the maxlength so we can include the null terminator */
	maxLen--;

	while (*ptr != '\0' && (browsed <= textLen - searchLen))
	{
		/* See if we get a comparison */
		if ((caseSensitive ? strncmp(ptr, search, searchLen) : strncasecmp(ptr, search, searchLen)) == 0)
		{
			if (replaceLen > searchLen)
			{
				/* First, see if we have enough space to do this operation */
				if (maxLen - textLen < replaceLen - searchLen)
				{
					/* First, see if the replacement length goes out of bounds. */
					if (browsed + replaceLen >= maxLen)
					{
						/* EXAMPLE CASE:
						 * Subject: AABBBCCC
						 * Buffer : 12 bytes
						 * Search : BBB
						 * Replace: DDDDDDDDDD
						 * OUTPUT : AADDDDDDDDD
						 * POSITION:           ^
						 */
						/* If it does, we'll just bound the length and do a strcpy. */
						replaceLen = maxLen - browsed;
						/* Note, we add one to the final result for the null terminator */
						strncpy(ptr, replace, replaceLen+1);
					}
					else
					{
						/* EXAMPLE CASE:
						 * Subject: AABBBCCC
						 * Buffer : 12 bytes
						 * Search : BBB
						 * Replace: DDDDDDD
						 * OUTPUT : AADDDDDDDCC
						 * POSITION:         ^
						 */
						/* We're going to have some bytes left over... */
						size_t origBytesToCopy = (textLen - (browsed + searchLen)) + 1;
						size_t realBytesToCopy = (maxLen - (browsed + replaceLen)) + 1;
						char *moveFrom = ptr + searchLen + (origBytesToCopy - realBytesToCopy);
						char *moveTo = ptr + replaceLen;

						/* First, move our old data out of the way. */
						memmove(moveTo, moveFrom, realBytesToCopy);

						/* Now, do our replacement. */
						memcpy(ptr, replace, replaceLen);
					}
				}
				else
				{
					/* EXAMPLE CASE:
					 * Subject: AABBBCCC
					 * Buffer : 12 bytes
					 * Search : BBB
					 * Replace: DDDD
					 * OUTPUT : AADDDDCCC
					 * POSITION:      ^
					 */
					/* Yes, we have enough space.  Do a normal move operation. */
					char *moveFrom = ptr + searchLen;
					char *moveTo = ptr + replaceLen;

					/* First move our old data out of the way. */
					size_t bytesToCopy = (textLen - (browsed + searchLen)) + 1;
					memmove(moveTo, moveFrom, bytesToCopy);

					/* Now do our replacement. */
					memcpy(ptr, replace, replaceLen);
				}
			}
			else if (replaceLen < searchLen)
			{
				/* EXAMPLE CASE:
				 * Subject: AABBBCCC
				 * Buffer : 12 bytes
				 * Search : BBB
				 * Replace: D
				 * OUTPUT : AADCCC
				 * POSITION:   ^
				 */
				/* If the replacement does not grow the string length, we do not
				 * need to do any fancy checking at all.  Yay!
				 */
				char *moveFrom = ptr + searchLen;		/* Start after the search pointer */
				char *moveTo = ptr + replaceLen;		/* Copy to where the replacement ends */

				/* Copy our replacement in, if any */
				if (replaceLen)
				{
					memcpy(ptr, replace, replaceLen);
				}

				/* Figure out how many bytes to move down, including null terminator */
				size_t bytesToCopy = (textLen - (browsed + searchLen)) + 1;

				/* Move the rest of the string down */
				memmove(moveTo, moveFrom, bytesToCopy);
			}
			else
			{
				/* EXAMPLE CASE:
				 * Subject: AABBBCCC
				 * Buffer : 12 bytes
				 * Search : BBB
				 * Replace: DDD
				 * OUTPUT : AADDDCCC
				 * POSITION:     ^
				 */
				/* We don't have to move anything around, just do a straight copy */
				memcpy(ptr, replace, replaceLen);
			}

			return ptr + replaceLen;
		}
		ptr++;
		browsed++;
	}

	return NULL;
}


unsigned int ReplaceStringAll(char *subject, size_t maxlength, const char *search, const char *replace, bool caseSensitive)
{
	size_t searchLen = strlen(search);
	size_t replaceLen = strlen(replace);

	char *newptr, *ptr = subject;
	unsigned int total = 0;
	while ((newptr = ReplaceString(ptr, maxlength, search, searchLen, replace, replaceLen, caseSensitive)) != NULL)
	{
		total++;
		maxlength -= newptr - ptr;
		ptr = newptr;
		if (*ptr == '\0')
		{
			break;
		}
	}

	return total;
}

char *ReadFile(const char *mode, const char *fmt, ...)
{
	char filename[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(filename, sizeof(filename), fmt, args);
	va_end(args);

	FILE *pFile = fopen(filename, mode);
	assert(pFile);

	// obtain file size:
	fseek(pFile , 0 , SEEK_END);
	long size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *buffer = new char[size+1];
	fread(buffer, 1, size, pFile);
	buffer[size] = '\0';

	fclose(pFile);
	return buffer;
}

int WirteFile(const char *str, const char *mode, const char *fmt, ...)
{
	char filename[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(filename, sizeof(filename), fmt, args);
	va_end(args);

	FILE *pFile = fopen(filename, mode);
	assert(pFile);

	int result = fputs(str, pFile);
	fclose(pFile);
	return result;
}

bool StartsWith(const char *str, const char *prefix)
{
	size_t strLen = strlen(str);
	size_t prefixLen = strlen(prefix);

	if (prefixLen > strLen)
		return false;

	return strncmp(str, prefix, prefixLen) == 0;
}

bool EndsWith(const char *str, const char *suffix)
{
	size_t strLen = strlen(str);
	size_t suffixLen = strlen(suffix);

	if (suffixLen > strLen)
		return false;

	return strncmp(str + (strLen - suffixLen), suffix, suffixLen) == 0;
}

int SplitStringEx(char *src, const char *dest[], size_t maxStrings, const char *delimiter)
{
	if (!strstr(src, delimiter))
		return 0;

	int i = 0;
	char *token = strsep(&src, delimiter);
	while (token && i < maxStrings)
	{
		dest[i] = token;
		token = strsep(&src, delimiter);
		i++;
	}

	return i;
}

size_t FindString(const char *str, const std::vector<const char*> &array)
{
	for (size_t i = 0, len = array.size(); i < len; i++)
	{
		if (!strcmp(array[i], str))
			return i;
	}

	return -1;
}

void StringAppend(char **s1, const char *s2, const char *delimiter)
{
	if (*s1 == nullptr)
		*s1 = (char*)calloc(1, sizeof(char));

	char *buffer = strdup(*s1);
	size_t newLen = strlen(buffer) + strlen(s2) + strlen(delimiter) + 1;
	*s1 = (char*)realloc(*s1, newLen);

	sprintf(*s1, "%s%s%s", buffer, delimiter, s2);
	free(buffer);
}

} // namespace fdxx

