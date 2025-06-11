#ifndef STD_STRINGS_H
#define STD_STRINGS_H

#include "memory.h"
#include <stdint.h>

/**
 * String wrapper around raw char arrays. Elements should be accessed using
 * accessor functions rather than directly.
 */
typedef struct std_string {
  uint64_t _len;
  const char *_buf;
  int32_t _err;
} std_string;

/**
 * Initializes a dynamic string into [str] from a given null-terminated char
 * array constant [buf], allocating it to the arena [arena]. [buf] is copied
 * until it reaches the null terminator, or until [max] characters are hit.
 *
 * On an error, the [err] field of string is set to a non-zero value.
 */
std_string str_create(std_arena *arena, const char *buf);

/**
 * Initializes a string into [str] from a given null-terminated char array
 * constant [buf]. [buf] is copied until it reaches the null-terminator, or
 * until [max] characters are hit.
 *
 * This function assumes that the string buffer [buf] is externally managed.
 * Consider using this function over [str_create] when dealing with memory that
 * will be automatically freed, like stack-allocated strings or for read-only
 * strings that will never be modified.
 */
std_string str(const char *buf);

/**
 * Compares the two strings [a] and [b].
 * Returns 0 if they are equal, a positive integer if [a] > [b], and a negative
 * integer if [b] > [a].
 */
int str_compare(std_string a, std_string b);

/**
 * Get the substring of [str] starting at (inclusive) [from] to (exclusive)
 * [end]. Returns an empty string if [from] >= [to]. [from] and [to] must be
 * greater than 0.
 */
std_string str_substr(std_string str, uint64_t from, uint64_t to);

/**
 * Get the index location of the first occurrence of [c] in [str].
 */
uint64_t str_find(std_string str, char c);

/**
 * Get the length of string [str].
 */
uint64_t str_len(std_string str);

/**
 * Returns an empty string.
 */
std_string str_empty();

/**
 * Returns true iff the string is empty.
 */
bool str_isempty(std_string str);

/**
 * Get the value of the string [str].
 */
const char *str_get(std_string str);

/**
 * Get the character at [at] in [str]. Throws an error if [at] is greater than
 * the length of the string.
 */
char str_at(std_string str, uint64_t at);

/**
 * Returns an invalid string.
 */
std_string str_bad();

/**
 * Returns a non-zero value if the string is invalid, and 0 if it is.
 */
int32_t str_err(std_string str);

#endif // STD_STRINGS_H
