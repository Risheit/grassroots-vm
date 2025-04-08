/*
 * This file defines functions and structs necessary to run tests using the
 * `make test` command.
 *
 * Writing tests:
 *
 * Any .c files placed in the test folder are registered as a test file, and
 * will be run my `make test`. A standard test file should be comprised of:
 *
 * The include directive for this testing header file.
 *
 * ```
 * #include <testing.h>
 * ```
 *
 * A set of test functions. Test functions are functions that make use of the
 * test macros in this file, and are registered in the test files main function
 * (more on that later). Test functions can be added with the TEST macro:
 *
 * ```
 * TEST(test_name) { ... }
 * ```
 * test_name must be a valid function name.

 * before including the testing header file.
 *
 * The body of a testing function is written using the provided testing macros.
 * A standard test function might look like this:
 *
 * ```
 * TEST(testAddFunction) {
 *  int act = add(3, 2);
 *  IS_TRUE(act == 5);
 * }
 * ```
 *
 * The main function initializes the test data using INIT(), runs the defined
 * tests using RUN(test), then sends the test data back to the runner using
 * CONCLUDE(). A main function should generally look like:
 *
 * ```
 * int main() {
 *  INIT();
 *
 *  RUN(test1);
 *  RUN(test2);
 *  RUN(test3);
 *
 *  CONCLUDE();
 * }
 * ```
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define TEST_DNAME __std_test_data_main

// ---

#define PASS_TEST()                                                            \
  do {                                                                         \
    TEST_DNAME->state = testing_PASSED;                                        \
    return;                                                                    \
  } while (0)

#define FAIL_TEST(fail_message)                                                \
  do {                                                                         \
    TEST_DNAME->state = testing_FAILED;                                        \
    TEST_DNAME->failed++;                                                      \
    TEST_DNAME->message = fail_message;                                        \
    return;                                                                    \
  } while (0)

#define IS_TRUE(expression)                                                    \
  do {                                                                         \
    if (!(expression)) {                                                       \
      TEST_DNAME->state = testing_FAILED;                                      \
      TEST_DNAME->failed++;                                                    \
      TEST_DNAME->message =                                                    \
          __FILE__ __LINE__ #expression " was false when expected true";       \
      return;                                                                  \
    } else {                                                                   \
      TEST_DNAME->state = testing_PASSED;                                      \
    }                                                                          \
  } while (0)

#define IS_FALSE(expression)                                                   \
  do {                                                                         \
    if (expression) {                                                          \
      TEST_DNAME->state = testing_FAILED;                                      \
      TEST_DNAME->failed++;                                                    \
      TEST_DNAME->message =                                                    \
          __FILE __LINE__ #expression " was true when expected false";         \
      return;                                                                  \
    } else {                                                                   \
      TEST_DNAME->state = testing_PASSED;                                      \
    }                                                                          \
  } while (0)

// ---

#define INIT()                                                                 \
  std_test_data TEST_DNAME;                                                    \
  do {                                                                         \
    TEST_DNAME.failed = 0;                                                     \
    TEST_DNAME.run = 0;                                                        \
    TEST_DNAME.message = "";                                                   \
    TEST_DNAME.state = testing_NOT_RUN;                                        \
    fprintf(stderr, "Running tests for " __FILE__ ":\n");                      \
  } while (0)

#define RUN(testname)                                                          \
  do {                                                                         \
    TEST_DNAME.run++;                                                          \
    testname(&TEST_DNAME);                                                     \
    if (TEST_DNAME.state == testing_FAILED) {                                  \
      fprintf(stderr, __FILE__ ": Test %d failed: %s\n", TEST_DNAME.run,       \
              TEST_DNAME.message);                                             \
    }                                                                          \
    TEST_DNAME.state = testing_NOT_RUN;                                        \
  } while (0);

#define CONCLUDE()                                                             \
  do {                                                                         \
    fprintf(stderr, __FILE__ ": %d/%d failed tests.\n\n", TEST_DNAME.failed,   \
            TEST_DNAME.run);                                                   \
    return TEST_DNAME.failed;                                                  \
  } while (0)

#define TEST(testname) void testname(std_test_data *TEST_DNAME)

typedef enum std_test_state {
  testing_PASSED,
  testing_FAILED,
  testing_NOT_RUN,
} std_test_state;

typedef struct std_test_data {
  int32_t failed;
  int32_t run;
  const char *message;
  std_test_state state;
} std_test_data;

