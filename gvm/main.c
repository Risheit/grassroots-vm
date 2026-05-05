
/**
 * Step 1: Open GBC File for parsing
 * Step 2: Read until "code section begins" for metadata.
 * Step 3: Read data section into memory.
 * Step 4: Read instructions line by line.
 * Step 5: Perform based on instructions
 *
 */

#include "constants.h"
#include "std/cli.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"

int main(int argc, const char **argv) {
  std_string file_name = std_str_empty();
  std_argument arg;

  do {
    arg = std_cli_argv_next(argc, argv);

    if (arg.type == ARG_ARGUMENT) { // First arg is the name of the file.
      file_name = arg.argument.val;
      break;
    }
  } while (arg.type != ARG_END);

  // No file name passed in
  if (std_str_is_empty(file_name)) {
    std_eprintf("Invalid file name provided\n");
    return EXIT_BAD_ARGS;
  }

  std_arena *working_memory = std_dyn_arena();

  // Open GBC file for reading
  std_file *gbc_file = std_file_open(working_memory, file_name, FOPEN_READ, 0);
  if (std_file_err(gbc_file)) {
    std_errno_msg("Couldn't open GBC file");
    return EXIT_FILE_ERR;
  }

  std_szptr read = std_file_read(gbc_file, working_memory, 1, sizeof(int64_t));
  read = std_file_read(gbc_file, working_memory, 4, sizeof(byte));
  std_printf("%hhx %hhx\n", ((byte *)read.ptr)[0], ((byte *)read.ptr)[1]);

  std_arena_destroy(working_memory);

  return 0;
}
