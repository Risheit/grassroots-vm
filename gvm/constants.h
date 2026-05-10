#pragma once

#include <stdint.h>

// Read identifier as int64_t little-endian
#define GBC_IDENT 0x0A1A0D0A434247F7LL

#define GBC_VER_BYTES 4

#define GBC_VER_MAJOR 1
#define GBC_VER_MINOR 0
#define GBC_VER_PATCH 1

#define EXIT_BAD_ARGS 1
#define EXIT_FILE_ERR 2

#define GBC_MAX_PAGE_SZ 4096
#define GBC_MAX_PAGES 2
#define GBC_MAX_REGS 256

#define GBC_REG_ZERO 0 
#define GBC_REG_PC 255 
#define GBC_REG_IF 254 
#define GBC_REG_EC 253

typedef uint64_t Register;
typedef uint64_t MemOffset;
typedef uint32_t Instr;
