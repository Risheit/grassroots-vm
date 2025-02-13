#include "strings.h"
#include "std/memory.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

string str_alloc(arena *arena, const char *buf) {
  size_t n = strlen(buf);
  char *memory = arena_alloc(arena, n * sizeof(buf));
  string str = {.len = n, .err = memory == NULL};

  if (!memory)
    return str;

  strncpy(memory, buf, n);
  str.buf = memory;
  return str;
}

string str_alloc_s(const char *buf) {
  size_t n = strlen(buf);
  return (string){.buf = buf, .len = n};
}

inline size_t len(string str) { return str.len; }

inline const char *get(string str) { return str.buf; }

inline char at(string str, size_t at) {
  assert(at >= 0);
  assert(at < str.len);

  return str.buf[at];
}

inline const char *from(string str, size_t from) {
  assert(from >= 0);
  assert(from < str.len);

  return str.buf + from;
}
