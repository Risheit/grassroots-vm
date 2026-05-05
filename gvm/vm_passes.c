#include "vm_passes.h"
#include "constants.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include <stdint.h>

#define ASSERT_VALID(file, read, expectedSize)                                 \
  do {                                                                         \
    if ((read).size < expectedSize) {                                          \
      if (std_file_err((file)) != FERR_EOF) {                                  \
        std_errno_msg("File read error");                                      \
      }                                                                        \
      std_eprintf("Unable to parse GBC file.\n");                              \
      std_exit(EXIT_FILE_ERR);                                                 \
    }                                                                          \
  } while (0)

void consumeIdentifier(std_arena *restrict arena, std_file *restrict gbc_file) {
  std_szptr read = std_file_read(gbc_file, arena, 1, sizeof(int64_t));

  ASSERT_VALID(gbc_file, read, sizeof(int64_t));

  int64_t *ident = read.ptr;
  if (*ident != GBC_IDENT) {
    std_eprintf("File is not a GBC file!\n");
    std_exit(EXIT_FILE_ERR);
  }
}

void consumeVersionNumber(std_arena *restrict arena,
                          std_file *restrict gbc_file) {
  std_szptr read = std_file_read(gbc_file, arena, GBC_VER_BYTES, sizeof(byte));

  ASSERT_VALID(gbc_file, read, 4 * sizeof(byte));

  byte *version = read.ptr;

  bool isIncorrectVersion =
      version[0] != GBC_VER_MAJOR || version[1] != GBC_VER_MINOR;
  if (isIncorrectVersion) {
    std_eprintf(
        "GBC file is incorrect version: is v%d.%d.%d, needs v%d.0 - v%d.%d!\n",
        version[0], version[1], version[2], GBC_VER_MAJOR, GBC_VER_MAJOR,
        GBC_VER_MINOR);
    std_exit(EXIT_FILE_ERR);
  }
}
