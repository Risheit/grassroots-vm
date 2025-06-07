#ifndef STD_ERROR_H
#define STD_ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

/**
 * Prints formatted output to standard error.
 */
inline int eprintf(const char *restrict format, ...) {
  va_list args;
  va_start(args, format);
  int ret = vfprintf(stderr, format, args);
  va_end(args);

  return ret;
}

#endif // STD_ERROR_H
