/*
 * Defines the test runner executable.
 *
 * Any single test file should compile into a single executable file.
 * The test executable takes in all test executable names and runs them in
 * sequence. Tests files executables should return the total number of failing
 * tests. E.x. A working test file should return 0, while a test file containing
 * 2 failing tests should return 2.
 *
 * Use [test -h] or [test --help] for more information about the flags
 * the test runner can take.
 *
 * See ext/testing.h for more information about writing test files.
 */

#include "std/cli.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {

  // Parse the options given to find any important flags and ensure valid
  // input.
  std_argument arg = cli_argv_next(argc, argv);
  while (arg.type != ARG_END) {
    if (is_option(arg, str("-h")) || is_option(arg, str("--help"))) {
    }
  }

  int32_t failed_tests = 0;
  fprintf(stderr, "\nRunning Tests:\n\n");
  for (int i = 1; i < argc; i++) {
    int32_t ret = system(argv[i]);
    if (WIFEXITED(ret)) {
      failed_tests += WEXITSTATUS(ret);
    }
  }

  if (failed_tests == 0) {
    fprintf(stderr, "\nThere were no failing tests!\n");
  } else {
    fprintf(stderr, "\nIn total, there were %d failing tests.\n", failed_tests);
  }

  return 0;
}
