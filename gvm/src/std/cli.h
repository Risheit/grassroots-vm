#ifndef STD_CLI_H
#define STD_CLI_H

#include "std/strings.h"

typedef enum std_arg_type {
  ARG_OPTION,   // Is command line option (-a, --long)
  ARG_ARGUMENT, // Is command line argument (arg1)
  ARG_END,      // No further arguments or options available to parse.
} std_arg_type;

/**
 * Argument holder, which contains a command line option or argument.
 * If [type] is [ARG_OPTION], then the union [option] is available.
 * If [type] is [ARG_ARGUMENT], then the union [argument] is available.
 */
typedef struct std_argument {
  union {
    struct {
      std_string name;
      std_string arg;
      bool has_arg;
    } option;
    struct {
      std_string val;
    } argument;
  };
  std_arg_type type;
} std_argument;

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
std_argument cli_argv_next(int argc, const char **argv);

/**
 * Resets the state of argument parsing. Calling [next_argv] after calling this
 * function will begin argument parsing from the first argument.
 */
void cli_argv_reset();

#endif // STD_CLI_H
