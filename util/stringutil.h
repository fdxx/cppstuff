#pragma once

#include <vector>

namespace fdxx {

/**
 * Given a string, replaces the first occurrence of a search string with a replacement string.
 *
 * @param text          String to perform search and replacements on.
 * @param maxlength     Maximum length of the string buffer.
 *	                    Must be large enough to hold the size of the string after replacement.
 * @param search        String to search for.
 * @param replace       String to replace the search string with.
 * @param searchLen     searchLen.
 * @param replaceLen    replaceLen.
 * @param caseSensitive If true (default), search is case sensitive.
 * @return              Return the end position of the replaced string.
 */
char *ReplaceString(char *text, size_t maxlength, const char *search, size_t searchLen, const char *replace, size_t replaceLen, bool caseSensitive = true);

/**
 * Given a string, replaces all occurrences of a search string with a replacement string.
 *
 * @param text          String to perform search and replacements on.
 * @param maxlength     Maximum length of the string buffer.
 * @param search        String to search for.
 * @param replace       String to replace the search string with.
 * @param caseSensitive If true (default), search is case sensitive.
 * @return              Number of replacements that were performed.
 */
unsigned int ReplaceStringAll(char *text, size_t maxlength, const char *search, const char *replace, bool caseSensitive = true);

/**
 * Reading a string from a file
 *
 * @param mode          C string containing a file access mode.
 * @param fmt           Filename formatting rules.
 * @return              The string to read. Need to call delete[] to free memory.
 */
char *ReadFile(const char *mode, const char *fmt, ...);

/**
 * Write a string to a file
 *
 * @param str           Write string.
 * @param mode          C string containing a file access mode.
 * @param fmt           Filename formatting rules.
 * @return              On success, a non-negative value is returned.
 */
int WirteFile(const char *str, const char *mode, const char *fmt, ...);


// Checks if the string starts with the specified prefix.
bool StartsWith(const char *str, const char *prefix);

// Checks if the string ends with the specified suffix.
bool EndsWith(const char *str, const char *suffix);


/**
 * Splits a string into substrings according to the specified delimiter,
 * and stores these substrings in the target string array.
 *
 * @param src           Source string. After splitting,
 *                      the delimiter in the string will be replaced by a '\0' terminator.
 * @param dest          An array of destination strings that store the split substrings.
 * @param maxStrings    Maximum number of substrings to split.
 * @param delimiter     delimiter.
 * @return              Number of split substrings.
 */
int SplitStringEx(char *src, const char *dest[], size_t maxStrings, const char *delimiter);

// Returns the index of the string in the array.
size_t FindString(const char *str, const std::vector<const char*> &array);


// Append string.
// If *s1 is not NULL, it must have been previously allocated by malloc(), etc.
// If *s1 is NULL, need to call free() to free the memory.
void StringAppend(char **s1, const char *s2, const char *delimiter);

}

