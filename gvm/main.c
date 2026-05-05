
/**
 * Step 1: Open GBC File for parsing
 * Step 2: Read until "code section begins" for metadata.
 * Step 3: Read data section into memory.
 * Step 4: Read instructions line by line.
 * Step 5: Perform based on instructions
 *
 */

#include "std/cli.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"

static void DumpHex(const void *data, size_t size) {
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    std_printf("%02X ", ((unsigned char *)data)[i]);
    if (((unsigned char *)data)[i] >= ' ' &&
        ((unsigned char *)data)[i] <= '~') {
      ascii[i % 16] = ((unsigned char *)data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i + 1) % 8 == 0 || i + 1 == size) {
      std_printf(" ");
      if ((i + 1) % 16 == 0) {
        std_printf("|  %s \n", ascii);
      } else if (i + 1 == size) {
        ascii[(i + 1) % 16] = '\0';
        if ((i + 1) % 16 <= 8) {
          std_printf(" ");
        }
        for (j = (i + 1) % 16; j < 16; ++j) {
          std_printf("   ");
        }
        std_printf("|  %s \n", ascii);
      }
    }
  }
}

int main(int argc, const char **argv) {
  std_string file_name;
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
    return 1;
  }

  std_arena *working_memory = std_dyn_arena();

  // Open GBC file for reading
  std_file *gbc_file = std_file_open(working_memory, file_name, FOPEN_READ, 0);
  if (std_file_err(gbc_file)) {
    std_errno_msg("Couldn't open GBC file");
    return 2;
  }

  size_t size = std_file_size(gbc_file);
  byte *ptr = std_arena_alloc(working_memory, size);
  size_t read = std_file_read(ptr, gbc_file, size);
  if (read < size) {
    std_eprintf("read < size: %lu < %lu\n", read, size);
  }
  if (std_file_err(gbc_file)) {
    return 3;
  }

  DumpHex(ptr, read);

  return 0;
}
