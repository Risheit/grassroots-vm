/*
 * The file where build configuration is handled.
 * See [seed.h] for more details.
 */

#define SEED_IMPLEMENTATION
#include "seed.h"

int main(void) {
  (void)sd_root_create("root", "src");
  sd_module *std = sd_module_create("std");

  sd_module_share(std);

  // Compilation instructions:
  //    For each foo.c in (shared) std:
  //      cc -c foo.c -o $root/build/foo.o
  //    Finally:
  //      cc -c main.c [(shared) foo.o bar.o ...] -o main
  sd_compile ();
}
