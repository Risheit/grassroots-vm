#ifndef __STD_STRINGS_H
#define __STD_STRINGS_H

#include "std/memory.h"
#include <stddef.h>
#include <stdint.h>

/**
 * String view wrapper around raw char arrays.
 * While access to the raw data parts are provided, avoid accessing internal
 * fields directly, instead use provided manipulation functions.
 *
 * The goal of string is to provide a clean, safe way to handle char arrays.
 */
struct __std_string {
  size_t len;
  const char *buf;
  uint8_t err;
};

typedef struct __std_string string;

/**
 * Initializes a dynamic string into [str] from a given null-terminated char
 * array constant [buf], allocating it to the arena [arena]. [buf] is copied
 * until it reaches the null terminator, or until [max] characters are hit.
 *
 * On an error, the [err] field of string is set to a non-zero value.
 */
string str_alloc(arena *arena, const char *buf);

/**
 * Initializes a string into [str] from a given null-terminated char array
 * constant [buf]. [buf] is copied until it reaches the null-terminator, or
 * until [max] characters are hit.
 *
 * This function assumes that the memory [buf] is externally managed. Consider
 * using this function over [str_alloc] when dealing with memory that will be
 * automatically freed, like stack-allocated strings.
 */
string str_alloc_s(const char *buf);

/**
 * Get the length of string [str].
 */
inline size_t len(string str);

/**
 * Get the value of the string [str].
 */
inline const char *get(string str);

/**
 * Get the character at [at] in [str].
 */
inline char at(string str, size_t at);

/**
 * Get the substring of [str] starting at [from].
 */
inline const char *from(string str, size_t from);

#endif // __STD_STRINGS_H
