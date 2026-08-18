
/**
 * Step 1: Open GBC File for parsing
 * Step 2: Read until "code section begins" for metadata.
 * Step 3: Read data section into memory.
 * Step 4: Read instructions line by line.
 * Step 5: Perform based on instructions
 *
 */

#include "Constants.h"
#include "VirtualMachine.h"
#include "std/cli.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"

int main(int argc, const char **argv) {
  std_string fileName = std_str_empty();
  std_argument arg;

  do {
    arg = std_cli_argv_next(argc, argv);

    if (arg.type == ARG_ARGUMENT) { // First arg is the name of the file.
      fileName = arg.argument.val;
      break;
    }
  } while (arg.type != ARG_END);

  // No file name passed in
  if (std_str_is_empty(fileName)) {
    std_eprintf("Invalid file name provided\n");
    return EXIT_BAD_ARGS;
  }

  std_with_arena(persistent, std_dyn_arena()) {
    std_with_file(gbcFile, std_file_open(persistent, fileName, FOPEN_READ, 0)) {
      if (std_file_err(gbcFile)) {
        std_errno_msg("Couldn't open GBC file");
        std_exit(EXIT_FILE_ERR);
      }

      runVirtualMachine(gbcFile);
    }
  }

  return 0;
}
