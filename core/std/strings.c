#include "strings.h"
#include "memory.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static std_string str_alloc_n(std_arena *arena, const char *buf, size_t n) {
  char *memory = arena_alloc(arena, n * sizeof(buf));
  std_string str = {.len = n, .err = memory == NULL};

  if (!memory)
    return str;

  strncpy(memory, buf, n);
  str.buf = memory;
  return str;
}

std_string str_create(std_arena *arena, const char *buf) {
  size_t n = strlen(buf);
  return str_alloc_n(arena, buf, n);
}

std_string str(const char *buf) {
  size_t n = strlen(buf);
  return (std_string){.buf = buf, .len = n};
}

int str_compare(std_string a, std_string b) {
  // Early length check to try and avoid full string comp
  if (len(a) != len(b))
    return len(a) > len(b) ? 1 : -1;

  return strncmp(get(a), get(b), len(a));
}

std_string str_substr(std_string str, size_t from, size_t to) {
  assert(from <= len(str));
  assert(from >= 0);
  assert(to <= len(str));
  assert(to >= 0);

  if (from >= to)
    return STR_EMPTY;

  return (std_string){.buf = get(str) + from, .len = to - from};
}

size_t str_find(std_string str, char c) {
  size_t i = 0;
  for (; i < len(str); i++) {
    if (at(str, i) == c)
      return i;
  }
  return i;
}

extern inline size_t len(std_string str); 

extern inline bool str_empty(std_string str);

extern inline const char *get(std_string str); 

extern inline char at(std_string str, size_t at); 

