#ifndef __STD_CLI_H
#define __STD_CLI_H

#include "std/strings.h"
#include <stdbool.h>

enum __std_arg_type {
  ARG_OPTION,   // Is command line option (-a, --long)
  ARG_ARGUMENT, // Is command line argument (arg1)
  ARG_END,      // Argument parsing is complete, no further arguments or options
                // available to parse.
};

typedef enum __std_arg_type arg_type;

struct __std_argument {
  union {
    struct {
      string name;
      string arg;
      bool has_arg;
    } option;
    struct {
      string val;
    } argument;
  };
  arg_type type;
};

/**
 * Argument holder, which contains a command line option or argument.
 * If [type] is [ARG_OPTION], then the union [option] is available.
 * If [type] is [ARG_ARGUMENT], then the union [argument] is available.
 */
typedef struct __std_argument argument;

/**
 * Returns the next argument or option unparsed within [argv]. Repeated calls
 * will let you iterate through passed arguments. If all [argc] arguments in
 * [argv] have been parsed, all further calls will return an argument of the
 * [ARG_END] type, with no contained values.
 *
 * Options are any strings that
 * begin with "-", excluding "--". Options that have only one preceding "-" are
 * considered short options, while those with multiple preceding "-" are
 * considered long arguments.
 *
 * All other strings are considered arguments. Any
 * CLI arguments provided after a "--" are considered arguments, including
 * strings normally considered options.
 *
 * Options can be provided with option arguments.
 * * [-farg] or [-f arg] provide the [-f] short option with the argument [arg].
 * * [--long=arg] or [--long arg] provide the [--long] long option with the
 * argument [arg].
 */
argument next_argv(int argc, const char **argv);

/**
 * Resets the state of argument parsing. Calling [next_argv] after calling this
 * function will begin argument parsing from the first argument.
 */
void reset_arg_parse();

#endif // __STD_CLI_H
