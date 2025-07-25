#include "error.h"

#include <stdio.h>
#include <stdlib.h>

__attribute__((format(printf, 1, 2))) int eprintf(const char *restrict format,
                                                  ...) {
  va_list args;
  va_start(args, format);
  int ret = vfprintf(stderr, format, args);
  va_end(args);

  return ret;
}

_Noreturn void _std_builtin_assert(const char *filename, const char *func,
                                   int line, int expr, const char *err,
                                   const char *format, ...) {
  eprintf("Assertion failed: function %s, file %s, line %d\n%s: .\n", func,
          filename, line, err);

  va_list args;
  va_start(args, format);
  int ret = vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, ".\n");
  abort();
}

_Noreturn void _std_builtin_panic(const char *filename, const char *func,
                                  int line, const char *format, ...) {
  eprintf("Panic:");

  va_list args;
  va_start(args, format);
  int ret = vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, ".\n");
  abort();
}
