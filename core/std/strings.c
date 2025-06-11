#include "strings.h"
#include "memory.h"
#include <_static_assert.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

/**
 * Internal data type for std_string handle type.
 */
typedef struct str_data {
  uint64_t len;
  const char *buf;
  int32_t err;
} str_data;

#define RAW(handle) ((str_data *)(handle).raw)[0]
#define SET_STR(string, data, n, error)                                        \
  memcpy(string.raw, &(str_data){.buf = data, .len = n, .err = error},         \
         _STD_STRING_RAW_SIZE)
#define STR_VALID(str) assert(!str_err(str))

static_assert(sizeof(std_string) >= sizeof(str_data),
              "String handle is smaller than data type. Increase the "
              "string handle size in the strings.h header file.");
static_assert(alignof(std_string) == alignof(str_data),
              "String handle alignment is different from string type "
              "alignment. Modify the string alignment in strings.h");

static std_string str_alloc_n(std_arena *arena, const char *buf, uint64_t n) {
  char *memory = arena_alloc(arena, n * sizeof(buf));

  std_string string;
  SET_STR(string, NULL, n, !memory);

  if (!memory)
    return string;

  strncpy(memory, buf, n);
  RAW(string).buf = memory;
  return string;
}

std_string str_create(std_arena *arena, const char *buf) {
  uint64_t n = strlen(buf);
  return str_alloc_n(arena, buf, n);
}

std_string str(const char *buf) {
  uint64_t n = strlen(buf);
  std_string string;
  SET_STR(string, buf, n, false);
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

  std_string string;
  uint64_t len = to > str_len(str) ? str_len(str) : to;
  SET_STR(string, str_get(str) + from, len - from, false);

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

  return RAW(str).len;
}

std_string str_empty() {
  std_string string;
  SET_STR(string, "", 0, false);
  return string;
}

bool str_isempty(std_string str) {
  STR_VALID(str);
  return str_len(str) == 0;
}

const char *str_get(std_string str) {
  STR_VALID(str);
  return RAW(str).buf;
}

char str_at(std_string str, uint64_t at) {
  STR_VALID(str);
  assert(at >= 0);
  assert(at < RAW(str).len);

  return RAW(str).buf[at];
}

std_string str_bad() {
  std_string string;
  SET_STR(string, "", 0, true);
  return string;
}

int32_t str_err(std_string str) { return RAW(str).err; }
