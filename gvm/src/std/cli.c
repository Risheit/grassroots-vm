#include "cli.h"
#include "std/strings.h"

struct option get_next_opt(int argc, const char **argv) {
  static int cur = 1;
  string arg = str_alloc_s(argv[cur]);


}
