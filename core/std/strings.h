#ifndef STD_STRINGS_H
#define STD_STRINGS_H

#include "memory.h"
#include <stddef.h>
#include <stdint.h>

/**
 * String wrapper around raw char arrays. Elements should be accessed using
 * accessor functions rather than directly.
 */
typedef struct std_string {
  size_t _len;
  const char *_buf;
  int _err;
} std_string;

enum {
  STERR_OMEM = 1, // Out of memory to allocate string.
  STERR_BIG = 2,  // String too big to be stored (length field overflows).
  STERR_TPOT = 3, // No reason given for bad string.
  STERR_READ = 4  // Couldn't read into the string. Used for File IO.
};

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
std_string str_substr(std_string str, size_t from, size_t to);

/**
 * Get the index location of the first occurrence of [c] in [str].
 */
size_t str_find(std_string str, char c);

/**
 * Get the length of string [str].
 */
size_t str_len(std_string str);

/**
 * Appends [left] to [right] and stores it in [arena]. If [arena] cannot store
 * the appended strings, an error string is returned.
 */
std_string str_append(std_arena *arena, std_string left, std_string right);

/**
 * Returns an empty string with length 0.
 */
std_string str_empty();

/**
 * Returns a null string. Unlike an empty string, this string has a size of 1,
 * but only contains null-terminator characters. Use this to read an explicit
 * null-terminator into an [std_string] if needed.
 */
std_string str_null();

/**
 * Returns true iff the string is empty.
 */
bool str_isempty(std_string str);

/**
 * Get the underlying buffer of the string [str].
 * Note that this buffer is not guaranteed to be null-terminated! Cap all
 * interactions with raw buffers with the string's length using [str_len].
 */
const char *str_get(std_string str);

/**
 * Get the character at [at] in [str]. Throws an error if [at] is greater than
 * the length of the string.
 */
char str_at(std_string str, size_t at);

/**
 * Returns an invalid string.
 */
std_string str_bad();

/**
 * Returns an invalid string with a specific error code.
 */
std_string str_bad_ped(int err);

/**
 * Returns the error code of the string if it is invalid, and 0 if there is no
 * problem. See associated [STERR_] string error codes in this header file for
 * the meaning of each code.
 */
int str_err(std_string str);

#endif // STD_STRINGS_H
