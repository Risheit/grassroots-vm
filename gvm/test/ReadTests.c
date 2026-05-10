#include "VirtualMachine.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/testing.h"

TEST(readValidGBCFile, std_arena *arena) {
  const MemOffset expectedLinkTable = 0x0;
  const MemOffset expectedCodeSection = 0x30;
  const MemOffset expectedTypeTable = 0x54;
  const MemOffset expectedDataSection = 0x62;
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/hello_world.gbc"), FOPEN_READ, 0);

  IS_TRUE(std_file_err(file) == 0);

  // Expect no panic
  consumeIdentifier(arena, file);
  consumeVersionNumber(arena, file);
  GBCOffsets data = consumeSectionOffsets(arena, file);

  IS_TRUE(data.linkTable == expectedLinkTable);
  IS_TRUE(data.codeSection == expectedCodeSection);
  IS_TRUE(data.typeTable == expectedTypeTable);
  IS_TRUE(data.dataSection == expectedDataSection);
}

TEST(readInvalidIdentifier, std_arena *arena) {
  std_file *file = std_file_open(arena, str(PREBUILT_DIR "/invalid_ident.gbc"),
                                 FOPEN_READ, 0);

  IS_TRUE(std_file_err(file) == 0);

  IS_PANIC(consumeIdentifier(arena, file));
}

TEST(readInvalidVersion, std_arena *arena) {
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/invalid_ver.gbc"), FOPEN_READ, 0);

  IS_TRUE(std_file_err(file) == 0);

  consumeIdentifier(arena, file);
  IS_PANIC(consumeVersionNumber(arena, file));
}

int main(int argc, char **argv) {
  INIT(argc, argv);

  std_arena *arena = std_dyn_arena();

  RUN(readValidGBCFile, arena);
  std_arena_clean(arena);

  RUN(readInvalidIdentifier, arena);
  std_arena_clean(arena);

  RUN(readInvalidVersion, arena);
  std_arena_clean(arena);

  std_arena_destroy(arena);
  CONCLUDE();
}
