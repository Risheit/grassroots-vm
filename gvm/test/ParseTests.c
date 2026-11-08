#include "Constants.h"
#include "VirtualMachine.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/testing.h"

#define HELLO_WORLD_INSTR_1 0x000E0116 // MOVEI 0 '1 14
#define HELLO_WORLD_INSTR_2 0x48010112 // PUSH '1
#define HELLO_WORLD_INSTR_3 0x00620202 // LOADI '2 0x62
#define HELLO_WORLD_INSTR_4 0x48020212 // PUSH '2

#define BIG_FILE_INSTR 0x01000005 // ADD '1 '0 '0

TEST(readValidCodePage, std_file *file) {
  Machine *machine = initVirtualMachine(file);
  readCodePage(machine, 0);
  PASS_TEST(); // Expect no errors.
}

TEST(consumeFirstInstruction, std_file *file) {

  Machine *machine = initVirtualMachine(file);
  MemOffset pcStart = *getProgramCounter(machine);

  Instr firstInstr = consumeNextInstruction(machine);
  IS_TRUE(firstInstr == HELLO_WORLD_INSTR_1);
  IS_TRUE(*getProgramCounter(machine) == pcStart + sizeof(Instr));

  destroyVirtualMachine(machine);
}

TEST(consumeMultipleInstructions, std_file *file) {
  Machine *machine = initVirtualMachine(file);
  MemOffset pcStart = *getProgramCounter(machine);

  Instr instr1 = consumeNextInstruction(machine);
  IS_TRUE(instr1 == HELLO_WORLD_INSTR_1);
  IS_TRUE(*getProgramCounter(machine) == pcStart + sizeof(Instr));

  Instr instr2 = consumeNextInstruction(machine);
  IS_TRUE(instr2 == HELLO_WORLD_INSTR_2);
  IS_TRUE(*getProgramCounter(machine) == pcStart + 2 * sizeof(Instr));

  Instr instr3 = consumeNextInstruction(machine);
  IS_TRUE(instr3 == HELLO_WORLD_INSTR_3);
  IS_TRUE(*getProgramCounter(machine) == pcStart + 3 * sizeof(Instr));

  Instr instr4 = consumeNextInstruction(machine);
  IS_TRUE(instr4 == HELLO_WORLD_INSTR_4);
  IS_TRUE(*getProgramCounter(machine) == pcStart + 4 * sizeof(Instr));

  destroyVirtualMachine(machine);
}

TEST(consumePastPageBoundary, std_arena *arena) {
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/big_file.gbc"), FOPEN_READ, 0);
  Machine *machine = initVirtualMachine(file);

  // Put program counter at page boundary
  *getProgramCounter(machine) += GBC_MAX_PAGE_SZ - sizeof(Instr);

  Instr instr = consumeNextInstruction(machine);
  IS_TRUE(instr == BIG_FILE_INSTR);
  instr = 0; // Reset to ensure file is being read correctly

  // At page boundary
  instr = consumeNextInstruction(machine);
  IS_TRUE(instr == BIG_FILE_INSTR);
  instr = 0;

  instr = consumeNextInstruction(machine);
  IS_TRUE(instr == BIG_FILE_INSTR);
  instr = 0;

  destroyVirtualMachine(machine);
  std_file_close(file);
}

int main(int argc, char **argv) {
  INIT(argc, argv);

  std_arena *arena = std_dyn_arena();
  std_file *hello_world_gbc =
      std_file_open(arena, str(PREBUILT_DIR "/hello_world.gbc"), FOPEN_READ, 0);

  RUN(readValidCodePage, hello_world_gbc);
  RUN(consumeFirstInstruction, hello_world_gbc);
  RUN(consumeMultipleInstructions, hello_world_gbc);
  std_file_close(hello_world_gbc);
  std_arena_clean(arena);

  RUN(consumePastPageBoundary, arena);

  CONCLUDE();
}
