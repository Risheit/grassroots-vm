#ifndef STD_ERROR_H
#define STD_ERROR_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * Prints formatted output to standard error.
 */
int eprintf(const char *restrict format, ...);

#ifdef __builtin_expect
#define _std_builtin_expect(e, v) __builtin_expect((e), (v))
#else
#define _std_builtin_expect(e, v) (e)
#endif

// TODO: Get this set up
#ifndef NDEBUG
#define std_assert(e, format, ...)                                             \
  (_std_builtin_expect(!(e), 0)                                                \
       ? _std_builtin_assert(__FILE_NAME__, __func__, __LINE__, e, #e,         \
                             format __VA_OPT__(, ) __VA_ARGS__)                \
       : (void)0)
#else
#define std_assert
#endif

#define std_nonnull(e) std_assert((e) != NULL, #e " should not be null")

/**
 * Standard library assert function. Use [std_assert].
 */
_Noreturn void _std_builtin_assert(const char *filename, const char *func,
                                   int line, int expr, const char *err,
                                   const char *format, ...);

#endif // STD_ERROR_H
