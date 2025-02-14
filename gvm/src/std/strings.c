#include "strings.h"
#include "std/memory.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static string str_alloc_n(arena *arena, const char *buf, size_t n) {
  char *memory = arena_alloc(arena, n * sizeof(buf));
  string str = {.len = n, .err = memory == NULL};

  if (!memory)
    return str;

  strncpy(memory, buf, n);
  str.buf = memory;
  return str;
}

string str_alloc(arena *arena, const char *buf) {
  size_t n = strlen(buf);
  return str_alloc_n(arena, buf, n);
}

string str(const char *buf) {
  size_t n = strlen(buf);
  return (string){.buf = buf, .len = n};
}

int str_compare(string a, string b) {
  // Early length check to try and avoid full string comp
  if (len(a) != len(b))
    return len(a) > len(b) ? 1 : -1;

  return strncmp(get(a), get(b), len(a));
}

string str_substr(string str, size_t from, size_t to) {
  assert(from <= len(str));
  assert(from >= 0);
  assert(to <= len(str));
  assert(to >= 0);

  if (from >= to)
    return str_empty;

  return (string){.buf = get(str) + from, .len = to - from};
}

size_t str_find(string str, char c) {
  size_t i = 0;
  for (; i < len(str); i++) {
    if (at(str, i) == c)
      return i;
  }
  return i;
}

extern inline size_t len(string str); 

/**
 * Get the value of the string [str].
 */
extern inline const char *get(string str); 

/**
 * Get the character at [at] in [str].
 */
extern inline char at(string str, size_t at); 

