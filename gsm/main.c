// GSM - The Grassroots ASM parser.

// Steps to convert to GBC:
//
// A. Get GA file from command line.
// B. Open GA file for reading.
//
// C. First Pass (We create a temporary intermediate .ga file):
// 1. Inline macros.
// 2. Remove comments.
// 3. Trim leading and trailing whitespace, and remove empty lines.
//
// D. Second Pass (We create a temporary .gbc file):
// 1. Add GBC metadata to binary file.
// 2. Traverse through data section (until main:)
//    2a. Write variable to GBC file.
//    2b. Store memory offset and reference to variable.
// 3. Traverse through code section (until EOF)
//    3a. If link: Store memory offset and reference to any link.
//    3b. If code: Convert into GBC, replace $vars and #links with memory
//    offset. Replace numbers with necessary binary (default to base 10 if no
//    0-alpha prefix)
// E. Copy temporary .gbc file to final .gbc file.
//
// On error:
// 1. Remove any created artefacts:
//    - Corrupt final .gbc file.

#include "assembler.h"
#include "std/cli.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"
#include <stdint.h>

int main(int argc, const char **argv) {
  std_string file_name = str_empty();

  // Parse CLI arguments

  std_argument arg;
  do {
    arg = cli_argv_next(argc, argv);

    if (arg.type == ARG_ARGUMENT) { // First arg is the name of the file.
      file_name = arg.argument.val;
      break;
    }
  } while (arg.type != ARG_END);

  // No file name passed in
  if (str_isempty(file_name)) {
    eprintf("Invalid file name provided\n");
    return 1;
  }

  std_arena *working_memory = arena_create(0, 0);

  // Open GA file for reading
  std_file *ga_file = file_open(working_memory, file_name, FOPEN_READ, 0);
  if (file_err(ga_file)) {
    errno_msg("Couldn't open GA file");
    return 2;
  }

  std_file *first_pass = file_temp(working_memory);
  if (file_err(first_pass)) {
    errno_msg("Couldn't create temporary file for assembler first pass");
    return 3;
  }

  std_file *out_file = file_open(working_memory, str("a.out"), FOPEN_WRITE, 0);
  if (file_err(out_file)) {
    errno_msg("Couldn't create output file");
    return 4;
  }

  // Copy GA file into temporary, inlining any macros.
  gsm_first_pass(ga_file, first_pass);

  // Create GBC file
  gsm_second_pass(first_pass, out_file);

  arena_destroy(working_memory);
}
