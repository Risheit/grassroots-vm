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
#include "std/error.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct test_node {
  std_string test_file;
  struct test_node *next;
} test_node;

int main(int argc, const char **argv) {
  std_arena *arena = arena_create(sizeof(test_node), 0);
  test_node *head = nullptr;

  // Parse the options given to find any important flags and ensure valid
  // input.
  std_argument arg = cli_argv_next(argc, argv);
  while (arg.type != ARG_END) {
    if (cli_is_option(arg, str("-h")) || cli_is_option(arg, str("--help"))) {
      printf("%s [-f file | --file=file] ... [-d dir | --dir=dir] ...\n",
             argv[0]);
      printf("-f, --file: Runs the given test file.\n");
      printf("-d, --dir: Runs all the files in [dir] as tests.\n");
    } else if (cli_is_option(arg, str("-f")) ||
               cli_is_option(arg, str("--file"))) {
      if (!arg.option.has_arg)
        std_panic("Missing argument filename for -f or --file option");

      test_node *new_test = arena_alloc(arena, sizeof *new_test);
      new_test->test_file = arg.option.arg;
      new_test->next = head;
      head = new_test;
    } else if (cli_is_option(arg, str("-d")) ||
               cli_is_option(arg, str("--dir"))) {
      if (!arg.option.has_arg)
        std_panic("Missing argument dirname for -f or --file option");
      // TODO: Implement directory parsing for test files.
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

  arena_destroy(arena);
  return 0;
}
