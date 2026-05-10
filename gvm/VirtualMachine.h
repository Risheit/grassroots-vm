#pragma once

#include "Constants.h"
#include "std/io.h"
#include "std/memory.h" // IWYU pragma: keep
#include <stdint.h>


typedef struct {
  MemOffset linkTable;   // lt_off
  MemOffset codeSection; // code_off
  MemOffset typeTable;   // type_off
  MemOffset dataSection; // data_off
} GBCOffsets;

// Runs the virtual machine over a given gbcFile. This returns a non-zero
// value on failure.
int runVirtualMachine(std_file *gbcFile);

#ifdef TEST_BUILD

// Reads and consumes a GBC file header from the start of [gbcFile]. Returns
// the section offsets read. If the header contains an invalid identifier or
// version number, this function exits with the program with an error.
GBCOffsets consumeSectionOffsets(std_arena *arena, std_file *gbcFile);

// Reads and consumes the GBC file identifier at the start of [gbcFile].
// Exits unless identifier is correct. Returns the identifier read.
void consumeIdentifier(std_arena *restrict arena, std_file *restrict gbcFile);

// Reads and consumes the GBC version number at the start of [gbcFile].
// Exits unless the version number is correct.
// Returns an array of length four allocated into [arena].
void consumeVersionNumber(std_arena *restrict arena,
                          std_file *restrict gbcFile);

#endif
