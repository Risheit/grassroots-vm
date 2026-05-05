#include "std/io.h"
#include "std/memory.h"
#include "std/testing.h"
#include "vm_passes.h"

TEST(readValidGBCFile) {
  std_arena *arena = std_dyn_arena();
  std_file *gbc_file =
      std_file_open(arena, str(PREBUILT_DIR "/hello_world.gbc"), FOPEN_READ, 0);

  IS_TRUE(std_file_err(gbc_file) == 0);

  // Expect no panic
  consumeIdentifier(arena, gbc_file);
  consumeVersionNumber(arena, gbc_file);

  std_arena_destroy(arena);
  PASS_TEST();
}

TEST(readInvalidIdentifier) {
  std_arena *arena = std_dyn_arena();
  std_file *gbc_file = std_file_open(
      arena, str(PREBUILT_DIR "/invalid_ident.gbc"), FOPEN_READ, 0);

  IS_TRUE(std_file_err(gbc_file) == 0);

  IS_PANIC(consumeIdentifier(arena, gbc_file));
  std_arena_destroy(arena);
}

TEST(readInvalidVersion) {
  std_arena *arena = std_dyn_arena();
  std_file *gbc_file =
      std_file_open(arena, str(PREBUILT_DIR "/invalid_ver.gbc"), FOPEN_READ, 0);

  IS_TRUE(std_file_err(gbc_file) == 0);

  consumeIdentifier(arena, gbc_file);
  IS_PANIC(consumeVersionNumber(arena, gbc_file));

  std_arena_destroy(arena);
}

int main(int argc, char **argv) {
  INIT(argc, argv);

  RUN(readValidGBCFile);
  RUN(readInvalidIdentifier);
  RUN(readInvalidVersion);

  CONCLUDE();
}
