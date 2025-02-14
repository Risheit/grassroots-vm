#include "cli.h"
#include "std/strings.h"
#include <stdbool.h>

static int opt_cur = 1;
static bool is_args_region = false;

static arg_type get_arg_type(string arg) {
  if (is_args_region)
    return ARG_ARGUMENT;

  return at(arg, 0) == '-' ? ARG_OPTION : ARG_ARGUMENT;
}

static argument parse_long_opt(string opt) {
  size_t split = str_find(opt, '=');
  // --long
  if (split == len(opt))
    return (argument){
        .type = ARG_OPTION,
        .option = {.arg = str_empty, .name = opt, .has_arg = false}};

  // --long=arg
  return (argument){.type = ARG_OPTION,
                    .option = {.arg = str_substr(opt, split + 1, len(opt)),
                               .name = str_substr(opt, 0, split),
                               .has_arg = true}};
}

static argument parse_short_opt(string opt) {
  // -farg
  if (len(opt) > 2)
    return (argument){.type = ARG_OPTION,
                      .option = {.arg = str_substr(opt, 2, len(opt)),
                                 .name = str_substr(opt, 0, 2),
                                 .has_arg = true}};

  // -f
  return (argument){
      .type = ARG_OPTION,
      .option = {.arg = str_empty, .name = opt, .has_arg = false}};
}

static argument parse_opt(string opt) {
  bool is_long_opt = str_compare(str_substr(opt, 0, 2), str("--")) == 0;
  return is_long_opt ? parse_long_opt(opt) : parse_short_opt(opt);
}

argument cli_argv_next(int argc, const char **argv) {
  // No more arguments to parse
  if (opt_cur >= argc)
    return (argument){.type = ARG_END};

  string this_arg = str(argv[opt_cur]);
  opt_cur++;

  // "--" represents beginning of argument only region.
  if (str_compare(this_arg, str("--")) == 0) {
    is_args_region = 1;
    this_arg = str(argv[opt_cur]);
    opt_cur++;
  }

  // option found
  if (get_arg_type(this_arg) == ARG_OPTION) {
    argument opt = parse_opt(this_arg);

    // Check if next CLI argument is an argument for this option, if the option
    // didn't come with an argument.

    if (opt_cur >= argc) // Skip if end of parsing
      return opt;

    string next_arg = str(argv[opt_cur]);
    if (!opt.option.has_arg && get_arg_type(next_arg) == ARG_ARGUMENT) {
      opt.option.has_arg = true;
      opt.option.arg = next_arg;
      opt_cur++;
    }

    return opt;
  }

  // argument found
  return (argument){.type = ARG_ARGUMENT, .argument = {.val = this_arg}};
}

void cli_argv_reset() {
  opt_cur = 1;
  is_args_region = false;
}
