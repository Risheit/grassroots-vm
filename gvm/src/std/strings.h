#ifndef STD_STRINGS_H
#define STD_STRINGS_H

#include "std/memory.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct std_string {
  size_t len;
  const char *buf;
  uint_fast8_t err;
};

/**
 * String view wrapper around raw char arrays.
 * While access to the raw data parts are provided, avoid accessing internal
 * fields directly, instead use provided manipulation functions.
 *
 * The goal of string is to provide a clean, safe way to handle char arrays.
 */
typedef struct std_string string;

static const string str_empty = {.buf = ""};

/**
 * Initializes a dynamic string into [str] from a given null-terminated char
 * array constant [buf], allocating it to the arena [arena]. [buf] is copied
 * until it reaches the null terminator, or until [max] characters are hit.
 *
 * On an error, the [err] field of string is set to a non-zero value.
 */
string str_create(arena *arena, const char *buf);

/**
 * Initializes a string into [str] from a given null-terminated char array
 * constant [buf]. [buf] is copied until it reaches the null-terminator, or
 * until [max] characters are hit.
 *
 * This function assumes that the string buffer [buf] is externally managed.
 * Consider using this function over [str_alloc] when dealing with memory that
 * will be automatically freed, like stack-allocated strings.
 */
string str_create_s(const char *buf);

#define str str_create_s

/**
 * Compares the two strings [a] and [b].
 * Returns 0 if they are equal, a positive integer if [a] > [b], and a negative
 * integer if [b] > [a].
 */
int str_compare(string a, string b);

/**
 * Get the substring of [str] starting at (inclusive) [from] to (exclusive)
 * [end].
 */
string str_substr(string str, size_t from, size_t to);

/**
 * Get the index location of the first occurrence of [c] in [str].
 */
size_t str_find(string str, char c);

/**
 * Get the length of string [str].
 */
inline size_t len(string str) { return str.len; }

/**
 * Get the value of the string [str].
 */
inline const char *get(string str) {
  assert(str.err == 0);

  return str.buf;
}

/**
 * Get the character at [at] in [str].
 */
inline char at(string str, size_t at) {
  assert(at >= 0);
  assert(at < str.len);

  return str.buf[at];
}

#endif // STD_STRINGS_H
