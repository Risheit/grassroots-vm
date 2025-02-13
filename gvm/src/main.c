
/**
 * Step 1: Open GBC File for parsing
 * Step 2: Read until "code section begins" for metadata.
 * Step 3: Read data section into memory.
 * Step 4: Read instructions line by line.
 * Step 5: Perform based on instructions
 *
 */

#include <stdio.h>

int main(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    printf("%s : ", argv[i]);
  }
  printf("\n");

  return 0;
}
