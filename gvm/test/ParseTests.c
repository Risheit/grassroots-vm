#include "Constants.h"
#include "VirtualMachine.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/testing.h"
#include <assert.h>

static const Instr helloInstr1 = GBC_createT1Instr(GBC_OP_MOVEI, 0, 1, 14);
static const Instr helloInstr2 = GBC_createT1Instr(GBC_OP_PUSH, 0, 1, 0);
static const Instr helloInstr3 = GBC_createT1Instr(GBC_OP_MOVEI, 0, 2, 0x62);
static const Instr helloInstr4 = GBC_createT1Instr(GBC_OP_PUSH, 0, 2, 0);
static const Instr bigFileInstr = GBC_createT3Instr(GBC_OP_ADD, 0, 1, 0, 0);

TEST(readValidCodePage, std_file *file) {
  Machine *machine = initVirtualMachine(file);
  readCodePage(machine, 0);
  PASS_TEST(); // Expect no errors.
}

TEST(consumeFirstInstruction, std_file *file) {

  Machine *machine = initVirtualMachine(file);
  MemOffset pcStart = *getProgramCounter(machine);

  Instr firstInstr = consumeNextInstruction(machine);
  IS_TRUE(firstInstr == helloInstr1);
  IS_TRUE(*getProgramCounter(machine) == pcStart + sizeof(Instr));

  destroyVirtualMachine(machine);
}

TEST(consumeMultipleInstructions, std_file *file) {
  Machine *machine = initVirtualMachine(file);
  MemOffset pcStart = *getProgramCounter(machine);

  Instr instr1 = consumeNextInstruction(machine);
  IS_TRUE(instr1 == helloInstr1);
  IS_TRUE(*getProgramCounter(machine) == pcStart + sizeof(Instr));

  Instr instr2 = consumeNextInstruction(machine);
  IS_TRUE(instr2 == helloInstr2);
  IS_TRUE(*getProgramCounter(machine) == pcStart + 2 * sizeof(Instr));

  Instr instr3 = consumeNextInstruction(machine);
  IS_TRUE(instr3 == helloInstr3);
  IS_TRUE(*getProgramCounter(machine) == pcStart + 3 * sizeof(Instr));

  Instr instr4 = consumeNextInstruction(machine);
  IS_TRUE(instr4 == helloInstr4);
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
  IS_TRUE(instr == bigFileInstr);
  instr = 0; // Reset to ensure file is being read correctly

  // At page boundary
  instr = consumeNextInstruction(machine);
  IS_TRUE(instr == bigFileInstr);
  instr = 0;

  instr = consumeNextInstruction(machine);
  IS_TRUE(instr == bigFileInstr);
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
