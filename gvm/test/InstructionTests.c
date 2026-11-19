#include "Constants.h"
#include "VirtualMachine.h"
#include "std/memory.h"
#include "std/testing.h"
#include <stdint.h>

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

TEST(runJUMP, Machine *machine) {
  static const uint16_t jumpAmt = 0xABAB;
  static const MemOffset initPcValue = 0xC0000000;
  static const Instr jumpUp = GBC_createT1Instr(GBC_OP_JUMP, 0, 0, jumpAmt);
  static const Instr jumpDown = GBC_createT1Instr(GBC_OP_JUMP, 1, 0, jumpAmt);
  static const Instr jumpUpIf = GBC_createT1Instr(GBC_OP_JUMP, 2, 0, jumpAmt);
  static const Instr jumpDownIf = GBC_createT1Instr(GBC_OP_JUMP, 3, 0, jumpAmt);

  Register *rIf = getRegister(machine, GBC_REG_IF);
  MemOffset *rPc = getProgramCounter(machine);

  *rPc = initPcValue;
  runInstruction(machine, jumpUp);
  IS_TRUE(*rPc == initPcValue + jumpAmt);

  *rPc = initPcValue;
  runInstruction(machine, jumpDown);
  IS_TRUE(*rPc == initPcValue - jumpAmt);

  *rPc = initPcValue;
  rIf->valu = 0;
  runInstruction(machine, jumpUpIf);
  IS_TRUE(*rPc == initPcValue);

  *rPc = initPcValue;
  rIf->valu = 1;
  runInstruction(machine, jumpUpIf);
  IS_TRUE(*rPc == initPcValue + jumpAmt);

  *rPc = initPcValue;
  rIf->valu = 88;
  runInstruction(machine, jumpUpIf);
  IS_TRUE(*rPc == initPcValue + jumpAmt);

  *rPc = initPcValue;
  rIf->valu = 0;
  runInstruction(machine, jumpDownIf);
  IS_TRUE(*rPc == initPcValue);

  *rPc = initPcValue;
  rIf->valu = 1;
  runInstruction(machine, jumpDownIf);
  IS_TRUE(*rPc == initPcValue - jumpAmt);

  *rPc = initPcValue;
  rIf->valu = 88;
  runInstruction(machine, jumpDownIf);
  IS_TRUE(*rPc == initPcValue - jumpAmt);
}

TEST(runJMPR, Machine *machine) {
  static const MemOffset initPcValue = 0xC0000000;
  static const MemOffset jumpPcValue = 0xDEADBEEF;
  static const Instr jmpr = GBC_createT1Instr(GBC_OP_JMPR, 0, 1, 0);
  static const Instr jmprIf = GBC_createT1Instr(GBC_OP_JMPR, 1, 1, 0);

  getRegister(machine, 1)->valu = jumpPcValue;
  Register *rIf = getRegister(machine, GBC_REG_IF);
  MemOffset *rPc = getProgramCounter(machine);

  *rPc = initPcValue;
  runInstruction(machine, jmpr);
  IS_TRUE(*rPc == jumpPcValue);

  *rPc = initPcValue;
  rIf->valu = 0;
  runInstruction(machine, jmprIf);
  IS_TRUE(*rPc == initPcValue);

  *rPc = initPcValue;
  rIf->valu = 1;
  runInstruction(machine, jmprIf);
  IS_TRUE(*rPc == jumpPcValue);

  *rPc = initPcValue;
  rIf->valu = 88;
  runInstruction(machine, jmprIf);
  IS_TRUE(*rPc == jumpPcValue);
}

TEST(runADD, Machine *machine) {
  Register *regA = getRegister(machine, 1);
  Register *regB = getRegister(machine, 2);
  Register *regC = getRegister(machine, 3);

  // uint: 10 + 5 = 15
  regA->valu = 0;
  regB->type = RT_uint;
  regB->valu = 10;
  regC->type = RT_uint;
  regC->valu = 5;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_ADD, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_uint);
  IS_TRUE(regA->valu == regB->valu + regC->valu);

  // int: 10 + (-3) = 7
  regA->vali = 0;
  regB->type = RT_int;
  regB->vali = 10;
  regC->type = RT_int;
  regC->vali = -3;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_ADD, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_int);
  IS_TRUE(regA->vali == regB->vali + regC->vali);

  // float: 3.14 + (-0.14) = 3.0
  regA->valf = 0.0;
  regB->type = RT_float;
  regB->valf = 3.14;
  regC->type = RT_float;
  regC->valf = -0.14;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_ADD, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_float);
  IS_TRUE(regA->valf == regB->valf + regC->valf);
}

TEST(runSUB, Machine *machine) {
  Register *regA = getRegister(machine, 1);
  Register *regB = getRegister(machine, 2);
  Register *regC = getRegister(machine, 3);

  // uint: 10 - 3 = 7
  regA->valu = 0;
  regB->type = RT_uint;
  regB->valu = 10;
  regC->type = RT_uint;
  regC->valu = 3;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_SUB, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_uint);
  IS_TRUE(regA->valu == regB->valu - regC->valu);

  // int: 10 - (-3) = 13
  regA->vali = 0;
  regB->type = RT_int;
  regB->vali = 10;
  regC->type = RT_int;
  regC->vali = -3;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_SUB, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_int);
  IS_TRUE(regA->vali == regB->vali - regC->vali);

  // float: 5.5 - 2.5 = 3.0
  regA->valf = 0.0;
  regB->type = RT_float;
  regB->valf = 5.5;
  regC->type = RT_float;
  regC->valf = 2.5;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_SUB, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_float);
  IS_TRUE(regA->valf == regB->valf - regC->valf);
}

TEST(runMUL, Machine *machine) {
  Register *regA = getRegister(machine, 1);
  Register *regB = getRegister(machine, 2);
  Register *regC = getRegister(machine, 3);

  // uint: 6 * 7 = 42
  regA->valu = 0;
  regB->type = RT_uint;
  regB->valu = 6;
  regC->type = RT_uint;
  regC->valu = 7;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_MUL, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_uint);
  IS_TRUE(regA->valu == regB->valu * regC->valu);

  // int: (-6) * 7 = -42
  regA->vali = 0;
  regB->type = RT_int;
  regB->vali = -6;
  regC->type = RT_int;
  regC->vali = 7;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_MUL, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_int);
  IS_TRUE(regA->vali == regB->vali * regC->vali);

  // float: 2.5 * 4.0 = 10.0
  regA->valf = 0.0;
  regB->type = RT_float;
  regB->valf = 2.5;
  regC->type = RT_float;
  regC->valf = 4.0;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_MUL, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_float);
  IS_TRUE(regA->valf == regB->valf * regC->valf);
}

TEST(runDIV, Machine *machine) {
  Register *regA = getRegister(machine, 1);
  Register *regB = getRegister(machine, 2);
  Register *regC = getRegister(machine, 3);

  // uint: 15 / 4 = 3
  regA->valu = 0;
  regB->type = RT_uint;
  regB->valu = 15;
  regC->type = RT_uint;
  regC->valu = 4;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_DIV, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_uint);
  IS_TRUE(regA->valu == regB->valu / regC->valu);

  // int: (-15) / 4 = -3
  regA->vali = 0;
  regB->type = RT_int;
  regB->vali = -15;
  regC->type = RT_int;
  regC->vali = 4;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_DIV, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_int);
  IS_TRUE(regA->vali == regB->vali / regC->vali);

  // float: 7.5 / 2.5 = 3.0
  regA->valf = 0.0;
  regB->type = RT_float;
  regB->valf = 7.5;
  regC->type = RT_float;
  regC->valf = 2.5;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_DIV, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_float);
  IS_TRUE(regA->valf == regB->valf / regC->valf);
}

TEST(runMOD, Machine *machine) {
  Register *regA = getRegister(machine, 1);
  Register *regB = getRegister(machine, 2);
  Register *regC = getRegister(machine, 3);

  // uint: 15 % 4 = 3
  regA->valu = 0;
  regB->type = RT_uint;
  regB->valu = 15;
  regC->type = RT_uint;
  regC->valu = 4;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_MOD, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_uint);
  IS_TRUE(regA->valu == regB->valu % regC->valu);

  // int: (-15) % 4 = -3
  regA->vali = 0;
  regB->type = RT_int;
  regB->vali = -15;
  regC->type = RT_int;
  regC->vali = 4;
  runInstruction(machine, GBC_createT3Instr(GBC_OP_MOD, 0, 1, 2, 3));
  IS_TRUE(regA->type == RT_int);
  IS_TRUE(regA->vali == regB->vali % regC->vali);

  // float: MOD on float should fail
  regA->valf = 0.0;
  regB->type = RT_float;
  regB->valf = 5.5;
  regC->type = RT_float;
  regC->valf = 2.0;
  IS_NONZERO_EXIT(
      runInstruction(machine, GBC_createT3Instr(GBC_OP_MOD, 0, 1, 2, 3)),
      EXIT_BAD_INSTR);
}

TEST(zeroRegNotModifiable, Machine *machine) {
  Register *regZero = getRegister(machine, GBC_REG_ZERO);
  Register *regB = getRegister(machine, 1);
  Register *regC = getRegister(machine, 2);

  regB->type = RT_uint;
  regB->valu = 10;
  regC->type = RT_uint;
  regC->valu = 5;

  runInstruction(machine, GBC_createT3Instr(GBC_OP_ADD, 0, 0, 1, 2));
  IS_TRUE(regZero->valu == 0);
  IS_TRUE(regZero->type == RT_uint);

  runInstruction(machine, GBC_createT3Instr(GBC_OP_SUB, 0, 0, 1, 2));
  IS_TRUE(regZero->valu == 0);
  IS_TRUE(regZero->type == RT_uint);

  runInstruction(machine, GBC_createT3Instr(GBC_OP_MUL, 0, 0, 1, 2));
  IS_TRUE(regZero->valu == 0);
  IS_TRUE(regZero->type == RT_uint);

  runInstruction(machine, GBC_createT3Instr(GBC_OP_DIV, 0, 0, 1, 2));
  IS_TRUE(regZero->valu == 0);
  IS_TRUE(regZero->type == RT_uint);

  runInstruction(machine, GBC_createT3Instr(GBC_OP_MOD, 0, 0, 1, 2));
  IS_TRUE(regZero->valu == 0);
  IS_TRUE(regZero->type == RT_uint);
}

int main(int argc, char **argv) {
  INIT(argc, argv);

  std_arena *arena = std_dyn_arena();
  std_file *file =
      std_file_open(arena, str(PREBUILT_DIR "/empty_data.gbc"), FOPEN_READ, 0);
  Machine *machine = initVirtualMachine(file);

  RUN(runLOADR, machine);

  MemOffset initrPc = *getProgramCounter(machine);
  RUN(runJUMP, machine);
  RUN(runJMPR, machine);
  *getProgramCounter(machine) = initrPc; // Reset PC after jumps modify it

  RUN(runADD, machine);
  RUN(runSUB, machine);
  RUN(runMUL, machine);
  RUN(runDIV, machine);
  RUN(runMOD, machine);

  CONCLUDE();
}
