#pragma once

#include "std/io.h"

// Performs a GSM first pass on [ga_file] and stores the result in [store].
// This function panics on a failure.
void gsm_first_pass(std_file *restrict ga_file, std_file *restrict store);

// Performs a GSM second pass on [ga_file] and stores the resulting binary in
// [gbc_file].
// This function panics on a failure.
void gsm_second_pass(std_file *restrict ga_file, std_file *restrict gbc_file);
