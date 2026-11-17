#include "Constants.h"
#include "VirtualMachine.h"
#include "std/memory.h"
#include "std/testing.h"

static const MemOffset uint8Address = 0xAE;

TEST(runLOADR, Machine *machine) {
  static const Instr loadrImm0 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 0);
  static const Instr loadrImm1 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 1);
  static const Instr loadrImm2 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 2);
  static const Instr loadrImm3 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 3);
  static const Instr loadrImm4 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 4);
  static const Instr loadrImm5 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 5);
  static const Instr loadrImm6 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 6);
  static const Instr loadrImm7 = GBC_createT2Instr(GBC_OP_LOADR, 1, 2, 7);

  Register *regDest = getRegister(machine, 1);
  Register *regSource = getRegister(machine, 2);

  // Manually set source register to point to address of uint8 in file
  regSource->valu = uint8Address;
  regSource->type = RT_uint;

  runInstruction(machine, loadrImm0);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF1F2F3F4F5F6F7F8);

  // endian-conversion means that lowest-order bytes are read first

  runInstruction(machine, loadrImm1);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF8);

  runInstruction(machine, loadrImm2);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF7F8);

  runInstruction(machine, loadrImm3);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF6F7F8);

  runInstruction(machine, loadrImm4);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF5F6F7F8);

  runInstruction(machine, loadrImm5);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF4F5F6F7F8);

  runInstruction(machine, loadrImm6);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF3F4F5F6F7F8);

  runInstruction(machine, loadrImm7);
  IS_TRUE(regDest->type == RT_uint);
  IS_TRUE(regDest->valu == 0xF2F3F4F5F6F7F8);
}

int main(int argc, char **argv) {
  INIT(argc, argv);

  std_arena *arena = std_dyn_arena();
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/empty_data.gbc"), FOPEN_READ, 0);
  Machine *machine = initVirtualMachine(file);

  RUN(runLOADR, machine);

  CONCLUDE();
}
