/* A quick sample test. See ext/testing.h for more details on writing tests. */

#include <testing.h>

void sample_passing_test(test_data *main) { PASS_TEST(); }

void sample_failing_test(test_data *main) {
  FAIL_TEST("This test always fails!");
}

int main() {
  INIT();

  RUN(sample_passing_test);
  RUN(sample_failing_test);

  CONCLUDE();
}
