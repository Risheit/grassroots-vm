// GSM - The Grassroots ASM parser.

// Steps to convert to GBC:
//
// A. Get GA file from command line.
// B. Open GA file for reading.
//
// C. First Pass (We create a temporary GA file):
// 1. Inline macros.
// 2. Remove comments.
// 3. Trim leading and trailing whitespace, and remove empty lines.
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
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"
#include <stdint.h>

const uint32_t g_gsm_guard = 0x0a0d0d0a;
const uint32_t g_gsm_magic = 0x4F434247;
const uint8_t g_gsm_major = 1;
const uint8_t g_gsm_minor = 0;
const uint8_t g_gsm_patch = 0;
const uint8_t g_gsm_unused = 0;

int32_t gsm_first_pass(std_file *restrict ga_file, std_file *restrict store);
int32_t gsm_second_pass(std_file *restrict ga_file,
                        std_file *restrict gbc_file);

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
  std_file ga_file = file_open(file_name, FOPEN_READ, 0);
  if (file_err(&ga_file)) {
    perror("Couldn't open GA file");
    return 2;
  }

  std_file first_pass = file_temp();
  if (file_err(&first_pass)) {
    perror("Couldn't create temporary file for assembler first pass");
    return 3;
  }

  std_file out_file = file_open(str("a.out"), FOPEN_WRITE, 0);
  if (file_err(&out_file)) {
    perror("Couldn't create output file");
    return 4;
  }

  // Copy GA file into temporary, inlining any macros.
  gsm_first_pass(&ga_file, &first_pass);

  // Create GBC file
  gsm_second_pass(&first_pass, &out_file);
}

// Performs a GSM first pass on [ga_file] and stores the result in [store].
// Returns 0 on a success, and a non-zero error code on failure.
int32_t gsm_first_pass(std_file *restrict ga_file, std_file *restrict store) {
  // Inline macros.

  // 1. Read line
  // 2. If line starts with ![WORD] [expand] then store [WORD] -> [expand].
  // 3. If line contains ![WORD] that has been found, replace it with [expand].

  // Remove comments.

  // Trim leading and trailing whitespace, and remove empty lines.

  return 0;
}

// Performs a GSM second pass on [ga_file] and stores the resulting binary in
// [gbc]. Returns 0 on a success, and a non-zero error code on failure.
int32_t gsm_second_pass(std_file *restrict ga_file,
                        std_file *restrict gbc_file) {

  // 1. Add GBC metadata to binary file.
  // 2. Traverse through data section (until main:)
  //    2a. Write variable to GBC file.
  //    2b. Store memory offset and reference to variable.
  // 3. Traverse through code section (until EOF)
  //    3a. If link: Store memory offset and reference to any link.
  //    3b. If code: Convert into GBC, replace $vars and #links with memory
  //    offset. Replace numbers with necessary binary (default to base 10 if no
  //    0-alpha prefix)

  // Add GBC file header (See GBC file spec for more details on these)

  file_write(&g_gsm_guard, sizeof g_gsm_guard, 1, gbc_file);
  file_write(&g_gsm_magic, sizeof g_gsm_magic, 1, gbc_file);
  file_write(&g_gsm_major, sizeof g_gsm_major, 1, gbc_file);
  file_write(&g_gsm_minor, sizeof g_gsm_minor, 1, gbc_file);
  file_write(&g_gsm_patch, sizeof g_gsm_patch, 1, gbc_file);
  file_write(&g_gsm_unused, sizeof g_gsm_unused, 1, gbc_file);

  long instr_start_mark = file_tell(gbc_file);

  file_seek(ga_file, 0, SEEK_SET);

  if (file_err(gbc_file))
    return file_err(gbc_file);

  // Begin reading lines from the GA file
  std_arena *line_memory = arena_create(0, 0);
  while (file_err(ga_file) == 0) {
    std_string line = file_read_line(line_memory, ga_file);

    std_str_token *token;
    char space = ' ';
    str_tokenize(token, line, space);

    // TODO: Read tokens from a GA line and write its bytecode to the gbc file.
  }

  return 0;
}
