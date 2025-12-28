#pragma once

#include <stdint.h>

// Type definitions for a GBC command. See
// https://github.com/Risheit/grassroots-vm/wiki/GBC-to-GA-Language-Specifications
typedef uint32_t gbc_command;

// Masks for various parts of a GBC command.

#define OPCODE_MASK 0x0000003F
#define OPCODE_SHFT 0

// Code 1 Masks

#define CODE1_FNC 0x000007C0
#define CODE1_FNC_SHFT 6

#define CODE1_RX1 0x0000F800
#define CODE1_RX1_SHFT 11

#define CODE1_IMM 0xFFFF0000
#define CODE1_IMM_SHFT 16

// Code 2 Masks

#define CODE2_RX1 0x000007C0
#define CODE2_RX1_SHFT 6

#define CODE2_RX2 0x0000F800
#define CODE2_RX2_SHFT 11

#define CODE2_IMM 0xFFFF0000
#define CODE2_IMM_SHFT 16

// Code 3 Masks

#define CODE3_FNC 0x00001FC0
#define CODE3_FNC_SHFT 6

#define CODE3_XXX 0x0001E000 // Unused bits
#define CODE3_XXX_SHFT 13

#define CODE3_RX1 0x003E0000
#define CODE3_RX1_SHFT 17

#define CODE3_RX2 0x07C00000
#define CODE3_RX2_SHFT 22

#define CODE3_RX3 0xF8000000
#define CODE3_RX3_SHFT 27

// Code 4 Masks

#define CODE4_XXX 0x00000040 // Unused bits
#define CODE4_XXX_SHFT 6

#define CODE4_IMM 0xFFFFFF00
#define CODE4_IMM_SHFT 8

// Opcode representations

// Code 2
#define OPCODE_CODE1 0x3F
#define OPCODE_CODE3 0x00
#define OPCODE_ADDI 0x02
#define OPCODE_SUBI 0x03
#define OPCODE_BLI 0x06
#define OPCODE_BRI 0x07
#define OPCODE_BRIU 0x08
#define OPCODE_ANDI 0x09
#define OPCODE_ORI 0x0A
#define OPCODE_XORI 0x0B
#define OPCODE_LW 0x0C
#define OPCODE_LH 0x0D
#define OPCODE_LB 0x0E
#define OPCODE_SW 0x10
#define OPCODE_SH 0x11
#define OPCODE_SB 0x12
#define OPCODE_EQ 0x13
#define OPCODE_GT 0x14
#define OPCODE_LT 0x15

// Code 4
#define OPCODE_SYS 0x0F

// Code 1 function representations

#define FUNC_LDU 0x01
#define FUNC_JR 0x02
#define FUNC_JRC 0x03
#define FUNC_JRCX 0x04
#define FUNC_PUSH 0x05
#define FUNC_POP 0x06
#define FUNC_ZERO 0x07

// Code 3 function representations

#define FUNC_ADD 0x01
#define FUNC_SUB 0x02
#define FUNC_MUL 0x03
#define FUNC_MULU 0x04
#define FUNC_DIV 0x05
#define FUNC_BLX 0x09
#define FUNC_BRX 0x0A
#define FUNC_BRXU 0x0B
#define FUNC_AND 0x0C
#define FUNC_OR 0x0D
#define FUNC_XOR 0x0E
#define FUNC_EQX 0x12
#define FUNC_GTX 0x13
#define FUNC_LTX 0x14
#define FUNC_ADDF 0x15
#define FUNC_SUBF 0x16
#define FUNC_MULF 0x17
#define FUNC_MULFU 0x18
#define FUNC_DIVF 0x19

// Register representations

#define R_AX 0x01
#define R_BX 0x02
#define R_CX 0x03
#define R_DX 0x04
#define R_EX 0x05
#define R_FX 0x06
#define R_GX 0x07
#define R_HX 0x08

#define R_SAX 0x09
#define R_SBX 0x0A
#define R_SCX 0x0B
#define R_SDX 0x0C

#define R_RET 0x0D
#define R_UP 0x0E
#define R_ZERO 0x0F
#define R_IF 0x10
#define R_TG 0x11
#define R_SP 0x12
#define R_SRG 0x13
#define R_ASM 0x14
#define R_PC 0x15

const uint32_t g_gsm_guard = 0x0a0d0d0a;
const uint32_t g_gsm_magic = 0x4F434247;
const uint8_t g_gsm_major = 0;
const uint8_t g_gsm_minor = 1;
const uint8_t g_gsm_patch = 0;
const uint8_t g_gsm_unused = 0;
