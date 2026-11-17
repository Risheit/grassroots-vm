#pragma once

#include <stdint.h>

typedef enum {
  RT_uint = 0, // Default initialization
  RT_int,
  RT_float,
} RegisterType;

typedef struct {
  RegisterType type;
  union {
    uint64_t valu; // Value as unsigned int
    int64_t vali;  // Value as signed int
    double valf;   // Value as 8-byte float
  };
} Register;

typedef uint64_t MemOffset;
typedef uint32_t Instr;

// Needs to be large enough to store all page index values (2^51 with 8192 bit
// pages)
typedef int64_t PageIndex;

// Read identifier as int64_t little-endian
#define GBC_IDENT 0x0A1A0D0A434247F7LL

#define GBC_VER_BYTES 4

#define GBC_VER_MAJOR 0
#define GBC_VER_MINOR 1
#define GBC_VER_PATCH 0

#define EXIT_BAD_ARGS 1  // Invalid arguments given to program
#define EXIT_FILE_ERR 2  // Error when parsing or reading file
#define EXIT_BAD_INSTR 3 // Invalid instruction given

#define GBC_MAX_PAGE_SZ 8192 // In bytes
#define GBC_MAX_CONCURRENT_PAGES 2

#define GBC_MAX_PAGE_INSTRS (GBC_MAX_PAGE_SZ / sizeof(Instr))

#define GBC_MAX_REGS 256
#define GBC_REG_ZERO 0
#define GBC_REG_PC 255
#define GBC_REG_IF 254
#define GBC_REG_EC 253

// Instruction field masks
#define GBC_OPCODE_MASK 0x0000003F
#define GBC_FUNC_MASK 0x000000C0
#define GBC_FUNC_SHIFT 6

// Type 1: immediate:16 | 'A:8   | func:2 | opcode:6
#define GBC_T1_IMM_MASK 0xFFFF0000
#define GBC_T1_REG_A_MASK 0x0000FF00
#define GBC_T1_IMM_SHIFT 16
#define GBC_T1_REG_A_SHIFT 8

// Type 2: immediate:10 | 'B:8 | 'A:8 | opcode:6
#define GBC_T2_IMM_MASK 0xFFC00000
#define GBC_T2_REG_A_MASK 0x00003FC0
#define GBC_T2_REG_B_MASK 0x003FC000
#define GBC_T2_IMM_SHIFT 22
#define GBC_T2_REG_A_SHIFT 6
#define GBC_T2_REG_B_SHIFT 14

// Type 3: 'C:8 | 'B:8 | 'A:8 | func:2 | opcode:6
#define GBC_T3_REG_A_MASK 0x0000FF00
#define GBC_T3_REG_B_MASK 0x00FF0000
#define GBC_T3_REG_C_MASK 0xFF000000
#define GBC_T3_REG_A_SHIFT 8
#define GBC_T3_REG_B_SHIFT 16
#define GBC_T3_REG_C_SHIFT 24

// Opcodes
#define GBC_OP_LOADR 0x01
#define GBC_OP_LOAD 0x02
#define GBC_OP_JUMP 0x03
#define GBC_OP_JMPR 0x04
#define GBC_OP_ADD 0x05
#define GBC_OP_SUB 0x06
#define GBC_OP_MUL 0x07
#define GBC_OP_DIV 0x08
#define GBC_OP_MOD 0x09
#define GBC_OP_NOT 0x0A
#define GBC_OP_AND 0x0B
#define GBC_OP_OR 0x0C
#define GBC_OP_XOR 0x0D
#define GBC_OP_SHIFT 0x0E
#define GBC_OP_LT 0x0F
#define GBC_OP_LE 0x10
#define GBC_OP_EQ 0x11
#define GBC_OP_PUSH 0x12
#define GBC_OP_POP 0x13
#define GBC_OP_CALL 0x14
#define GBC_OP_MOVE 0x15
#define GBC_OP_MOVEI 0x16
#define GBC_OP_TYPE 0x17
#define GBC_OP_EXIT 0x18

// Instruction creation macros - For testing purposes

#define GBC_createT1Instr(opcode, func, regA, imm)                             \
  ((opcode) | ((func) << GBC_FUNC_SHIFT) | ((regA) << GBC_T1_REG_A_SHIFT) |    \
   ((imm) << GBC_T1_IMM_SHIFT))

#define GBC_createT2Instr(opcode, regA, regB, imm)                             \
  ((opcode) | ((regA) << GBC_T2_REG_A_SHIFT) |                                 \
   ((regB) << GBC_T2_REG_B_SHIFT) | ((imm) << GBC_T2_IMM_SHIFT))

#define GBC_createT3Instr(opcode, func, regA, regB, regC)                      \
  ((opcode) | ((func) << GBC_FUNC_SHIFT) | ((regA) << GBC_T3_REG_A_SHIFT) |    \
   ((regB) << GBC_T3_REG_B_SHIFT) | ((regC) << GBC_T3_REG_C_SHIFT))
