#include "strings.h"
#include "error.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STR_VALID(str)                                                         \
  std_assert(str_err(str) == 0, "String has an error code %d", str_err(str))

static std_string str_alloc_n(std_arena *arena, const char *buf, size_t n) {
  char *memory = arena_alloc(arena, n * sizeof(buf));

  std_string string = {
      ._buf = nullptr, ._len = n, ._err = memory == NULL ? STERR_OMEM : 0};

  if (!memory)
    return string;

  strncpy(memory, buf, n);
  string._buf = memory;
  return string;
}

std_string str_create(std_arena *arena, const char *buf) {
  size_t n = strlen(buf);
  return str_alloc_n(arena, buf, n);
}

std_string str_create_n(std_arena *arena, const char *buf, size_t n) {
  return str_alloc_n(arena, buf, n);
}

std_string str(const char *buf) {
  size_t n = strlen(buf);
  std_string string = {._buf = buf, ._len = n, ._err = 0};
  return string;
}

int str_compare(std_string a, std_string b) {
  // Early length check to try and avoid full string comp
  if (str_len(a) != str_len(b))
    return str_len(a) > str_len(b) ? 1 : -1;

  return strncmp(str_get(a), str_get(b), str_len(a));
}

std_string str_substr(std_string str, size_t from, size_t to) {
  STR_VALID(str);
  std_assert(from >= 0, "index must be greater than 0");
  std_assert(to >= 0, "index must be greater than 0");

  if (from >= to)
    return str_empty();

  size_t len = to > str_len(str) ? str_len(str) : to;
  std_string string = {
      ._buf = str_get(str) + from, ._len = len - from, ._err = false};

  return string;
}

struct std_str_token {
  size_t pos;         // The current position in the string.
  std_string value;   // The substring value.
  std_string *parent; // The parent string.
  char split;         // The split character
};

void str_tokenize(std_str_token *token, std_string string, char split) {
  STR_VALID(string);

  size_t start, end;
  for (start = 0; start < str_len(string); start++) {
    if (str_at(string, start) != split)
      break;
  }

  for (end = start; end < str_len(string); end++) {
    if (str_at(string, end) == split)
      break;
  }

  token->pos = end;
  token->parent = &string;
  token->value = str_substr(string, start, end);
  token->split = split;
}

bool str_token_next(std_str_token *token) {
  std_nonnull(token);

  if (token->pos > str_len(*token->parent)) {
    return false;
  }

  if (token->pos == str_len(*token->parent)) {
    token->pos++; // Increase past end of string so that token get calls can
                  // fail after end of string.
    return false;
  }

  size_t start, end;
  for (start = token->pos; start < str_len(*token->parent); start++) {
    if (str_at(*token->parent, start) != token->split)
      break;
  }

  for (end = start; end < str_len(*token->parent); end++) {
    if (str_at(*token->parent, end) == token->split)
      break;
  }

  token->value = str_substr(*token->parent, start, end);
  return true;
}

std_string str_token_get(std_str_token *token) {
  std_nonnull(token);
  if (token->pos > str_len(*token->parent))
    return str_bad_ped(STERR_READ);
  return token->value;
}

size_t str_find(std_string str, char c) {
  STR_VALID(str);

  size_t i;
  for (i = 0; i < str_len(str); i++) {
    if (str_at(str, i) == c)
      return i;
  }
  return i;
}

size_t str_len(std_string str) {
  STR_VALID(str);
  return str._len;
}

std_string str_append(std_arena *arena, std_string left, std_string right) {
  size_t size = str_len(left) + str_len(right);

  // Overflow with size.
  if (size < str_len(left) || size < str_len(right))
    return (std_string){._err = STERR_BIG};

  // Allocate enough space for left and right, then copy them into the buffer in
  // parts.
  char *res = arena_alloc(arena, size * sizeof(left._buf));
  if (res == NULL)
    return (std_string){._err = STERR_OMEM};

  strncpy(res, left._buf, str_len(left));
  strncpy(res + str_len(left), right._buf, str_len(right));
  return (std_string){._len = size, ._buf = res, ._err = 0};
}

std_string str_empty() {
  return (std_string){._buf = "", ._len = 0, ._err = 0};
}

std_string str_null() {
  return (std_string){._buf = "\0", ._len = 1, ._err = 0};
}

bool str_isempty(std_string str) {
  STR_VALID(str);
  return str_len(str) == 0;
}

const char *str_get(std_string str) {
  STR_VALID(str);
  return str._buf;
}

char str_at(std_string str, size_t at) {
  STR_VALID(str);
  std_assert(at < str._len, "index greater than string length");

  return str._buf[at];
}

std_string str_bad() {
  std_string string = {._buf = "", ._len = 0, ._err = STERR_TPOT};
  return string;
}

std_string str_bad_ped(int err) {
  std_string string = {._buf = "", ._len = 0, ._err = err};
  return string;
}

int str_err(std_string str) { return str._err; }
