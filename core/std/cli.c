#include "cli.h"
#include "strings.h"
#include <stdbool.h>

static int opt_cur = 1;
static bool is_args_region = false;

static std_arg_type get_arg_type(std_string arg) {
  if (is_args_region)
    return ARG_ARGUMENT;

  return str_at(arg, 0) == '-' ? ARG_OPTION : ARG_ARGUMENT;
}

static std_argument parse_long_opt(std_string opt) {
  size_t split = str_find(opt, '=');

  // Found argument of type --long
  if (split == str_len(opt))
    return (std_argument){
        .type = ARG_OPTION,
        .option = {.arg = str_empty(), .name = opt, .has_arg = false}};

  // Found argument of type --long=arg
  return (std_argument){.type = ARG_OPTION,
                    .option = {.arg = str_substr(opt, split + 1, str_len(opt)),
                               .name = str_substr(opt, 0, split),
                               .has_arg = true}};
}

static std_argument parse_short_opt(std_string opt) {
  
  // Found argument of type -farg
  if (str_len(opt) > 2)
    return (std_argument){.type = ARG_OPTION,
                      .option = {.arg = str_substr(opt, 2, str_len(opt)),
                                 .name = str_substr(opt, 0, 2),
                                 .has_arg = true}};

  // Found argument of type -f
  return (std_argument){
      .type = ARG_OPTION,
      .option = {.arg = str_empty(), .name = opt, .has_arg = false}};
}

static std_argument parse_opt(std_string opt) {
  bool is_long_opt = str_compare(str_substr(opt, 0, 2), str("--")) == 0;
  return is_long_opt ? parse_long_opt(opt) : parse_short_opt(opt);
}

std_argument cli_argv_next(int argc, const char **argv) {
  // No more arguments to parse
  if (opt_cur >= argc)
    return (std_argument){.type = ARG_END};

  std_string this_arg = str(argv[opt_cur]);
  opt_cur++;

  // "--" represents beginning of argument only region.
  if (str_compare(this_arg, str("--")) == 0) {
    is_args_region = 1;
    this_arg = str(argv[opt_cur]);
    opt_cur++;
  }

  // option found
  if (get_arg_type(this_arg) == ARG_OPTION) {
    std_argument opt = parse_opt(this_arg);
    return opt;
  }

  // argument found
  return (std_argument){.type = ARG_ARGUMENT, .argument = {.val = this_arg}};
}

void cli_argv_reset() {
  opt_cur = 1;
  is_args_region = false;
}
