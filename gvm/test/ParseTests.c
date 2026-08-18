#include "VirtualMachine.h"
#include "std/memory.h"
#include "std/testing.h"

TEST(readValidCodePage, std_file *file) {
  Machine *machine = initVirtualMachine(file);
  readCodePage(machine, 0);
  PASS_TEST(); // Expect no errors.
}


int main(int argc, char **argv) {
  INIT(argc, argv);

  std_arena *arena = std_dyn_arena();
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/hello_world.gbc"), FOPEN_READ, 0);
  RUN(readValidCodePage, file);

  CONCLUDE();
}
