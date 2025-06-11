// GSM - The Grassroots ASM parser.

// Steps to convert to GBC:
//
// A. Get GA file from command line.
// B. Open GA file for reading.
//
// C. First Pass (We create a temporary GA file):
// 1. Inline macros.
// 2. Remove comments.
//
// D. Second Pass (We create a .gbc file):
// 1. Add GBC metadata to binary file.
// 2. Traverse through data section (until main:)
//    2a. Write variable to GBC file.
//    2b. Store memory offset and reference to variable.
// 3. Traverse through code section (until EOF)
//    3a. If link: Store memory offset and reference to any link.
//    3b. If code: Convert into GBC, replace $vars and #links with memory
//    offset. Replace numbers with necessary binary (default to base 10 if no
//    0-alpha prefix)
//
// On error:
// 1. Remove any created artefacts:
//    - .gbc file.

#include "std/cli.h"
#include "std/error.h"
#include "std/file.h"
#include "std/strings.h"
#include <stdio.h>

void gsm_first_pass(std_file ga_file, std_file store);

int main(int argc, const char **argv) {
  std_string file_name = str_empty();

  // Parse CLI arguments

  std_argument arg;
  do {
    arg = cli_argv_next(argc, argv);

    if (arg.type == ARG_ARGUMENT) { // First arg is the name of the file.
      file_name = arg.argument.val;
    }
  } while (arg.type != ARG_END);

  // No file name passed in
  if (str_isempty(file_name)) {
    eprintf("Invalid file name provided\n");
    return 1;
  }

  // Open GA file for reading
  std_file ga_file = file_open(file_name, FOPEN_READ);
  if (file_err(ga_file)) {
    perror("Couldn't open file");
    return 1;
  }

  std_file first_pass = file_temp();
  if (file_err(first_pass)) {
    perror("Couldn't create temporary file for assembler first pass");
    return 1;
  }

  // Copy GA file into temporary, inlining any macros.
  gsm_first_pass(ga_file, first_pass);
}

// Performs a GSM first pass on [ga_file] and stores the result in [store].
void gsm_first_pass(std_file ga_file, std_file store) {
  // Inline macros.

  // 1. Read line
  // 2. If line starts with ![WORD] [expand] then store [WORD] -> [expand].
  // 3. If line contains ![WORD] that has been found, replace it with [expand].



  // Remove comments.
}
