#ifndef __STD_CLI_H
#define __STD_CLI_H

#include "std/strings.h"
#include <stdbool.h>

struct option {
  string name;
  string arg;
  bool has_arg;
};

struct option get_next_opt(int argc, char **argv);

#endif // __STD_CLI_H
