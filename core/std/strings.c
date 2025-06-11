#include "strings.h"
#include "memory.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define STR_VALID(str) assert(!str_err(str))

static std_string str_alloc_n(std_arena *arena, const char *buf, uint64_t n) {
  char *memory = arena_alloc(arena, n * sizeof(buf));

  std_string string = {._buf = nullptr, ._len = n, ._err = !memory};

  if (!memory)
    return string;

  strncpy(memory, buf, n);
  string._buf = memory;
  return string;
}

std_string str_create(std_arena *arena, const char *buf) {
  uint64_t n = strlen(buf);
  return str_alloc_n(arena, buf, n);
}

std_string str(const char *buf) {
  uint64_t n = strlen(buf);
  std_string string = {._buf = buf, ._len = n, ._err = false};
  return string;
}

int str_compare(std_string a, std_string b) {
  // Early length check to try and avoid full string comp
  if (str_len(a) != str_len(b))
    return str_len(a) > str_len(b) ? 1 : -1;

  return strncmp(str_get(a), str_get(b), str_len(a));
}

std_string str_substr(std_string str, uint64_t from, uint64_t to) {
  STR_VALID(str);
  assert(from >= 0);
  assert(to >= 0);

  if (from >= to)
    return str_empty();

  uint64_t len = to > str_len(str) ? str_len(str) : to;
  std_string string = {
      ._buf = str_get(str) + from, ._len = len - from, ._err = false};

  return string;
}

uint64_t str_find(std_string str, char c) {
  STR_VALID(str);

  uint64_t i;
  for (i = 0; i < str_len(str); i++) {
    if (str_at(str, i) == c)
      return i;
  }
  return i;
}

uint64_t str_len(std_string str) {
  STR_VALID(str);

  return str._len;
}

std_string str_empty() {
  std_string string = {._buf = "", ._len = 0, ._err = false};
  return string;
}

bool str_isempty(std_string str) {
  STR_VALID(str);
  return str_len(str) == 0;
}

const char *str_get(std_string str) {
  STR_VALID(str);
  return str._buf;
}

char str_at(std_string str, uint64_t at) {
  STR_VALID(str);
  assert(at >= 0);
  assert(at < str._len);

  return str._buf[at];
}

std_string str_bad() {
  std_string string = {._buf = "", ._len = 0, ._err = true};
  return string;
}

int32_t str_err(std_string str) { return str._err; }
