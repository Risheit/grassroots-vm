/** A VM based on the LC-3 architecture.
 *
 * @see: https://www.jmeiners.com/lc3-vm/
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef uint32_t arch_type;
// An unsigned long long is at least 64 bytes, making it safe for memory
// architecture.
enum { MEMORY_MAX = 1ULL << 32 };

arch_type *memory; // Heap-allocated VM memory.

// Registers
enum {
  R_R0 = 0, // General program registers
  R_R1,
  R_R2,
  R_R3,
  R_R4,
  R_R5,
  R_R6,
  R_R7,
  R_PC,           // Program Counter
  R_COND,         // Stores information about the previous calculation
  REGISTER_COUNT, // Used as a count for the number of registers. Should remain
                  // the last value.
};
arch_type registers[REGISTER_COUNT];

// Op-codes
enum {
  OP_BR = 0, // branch
  OP_ADD,    // add
  OP_LD,     // load
  OP_ST,     // store
  OP_JSR,    // jump register
  OP_AND,    // bitwise and
  OP_LDR,    // load register
  OP_STR,    // store register
  OP_RTI,    // unused
  OP_NOT,    // bitwise not
  OP_LDI,    // load indirect
  OP_STI,    // store indirect
  OP_JMP,    // jump
  OP_RES,    // reserved unused
  OP_LEA,    // load effective address
  OP_TRAP    // execute trap
};

// Condition Flags
enum {
  FL_POS = 1 << 0, // P
  FL_ZRO = 1 << 1, // Z
  FL_NEG = 1 << 2, // N
};

int main() {
  memory = (arch_type *)malloc(sizeof(arch_type) * MEMORY_MAX);

  if (memory == NULL) {
    return 1;
  }

  free(memory);
}
