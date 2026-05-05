#pragma once

#include "std/io.h"
#include "std/memory.h"

// Reads and consumes the GBC file identifier at the start of [gbc_file].
// Exits unless identifier is correct.
void consumeIdentifier(std_arena *restrict arena, std_file *restrict gbc_file);

// Reads and consumes the GBC version number at the start of [gbc_file].
// Exits unless the version number is correct.
void consumeVersionNumber(std_arena *restrict arena,
                          std_file *restrict gbc_file);
