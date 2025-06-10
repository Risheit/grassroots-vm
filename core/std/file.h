#ifndef STD_FILE_H
#define STD_FILE_H

#include "strings.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>

// Size of underlying raw data. This field can be changed without notice, and
// shouldn't be referred to externally.
#define _STD_FILE_RAW_SIZE (sizeof(void *) * 2)

/**
 * Opaque handle type for files.
 */
typedef struct std_file {
  byte raw[_STD_FILE_RAW_SIZE];
} std_file;

/**
 * Open states of a file. These are equivalent to fopen states.
 */
typedef enum std_fopen_state {
  FOPEN_READ,     // Equivalent to "r"
  FOPEN_READPL,   // Equivalent to "r+"
  FOPEN_WRITE,    // Equivalent to "w"
  FOPEN_WRITEPL,  // Equivalent to "w+"
  FOPEN_APPEND,   // Equivalent to "a"
  FOPEN_APPENDPL, // Equivalent to "a+"
} std_fopen_state;

/**
 * Opens a file. On a failure, errno is set as specified by [fopen], and the
 * file returned is inactive and marked with the relevant error number.
 */
std_file file_open(std_string name, std_fopen_state state);

/**
 * Opens a temporary file in accordance with the function [tmpfile].
 * On a failure, errno is set as specified by [tmpfile] and the file returned is
 * inactive and marked with the relevant error number.
 */
std_file file_temp();

/**
 * Closes a file. On a failure, errno is set as specified by [fclose], and
 * [file] is marked with the relevant error number. Irregardless of success, the
 * file object is marked inactive. If an inactive file is closed, nothing
 * occurs, and [file] is marked with -1.
 */
void file_close(std_file *file);

/**
 * Returns the error code associated with [file]. This error code is set by
 * functions the last function to act on this file. The error code is equivalent
 * to errno values set by the called function. If the last function call on this
 * file succeeded, then this function returns 0.
 *
 * Note: It is possible for a file to have an error of 0 in spite of having an
 * had an error called on it, if a subsequent function call on the file
 * succeeded. It is recommended to check the error of the file after all
 * important function calls.
 */
int32_t file_err(std_file file);

/**
 * Returns true if [file] is active, and false if not. Files are marked
 * inactive after they are closed, or if a critical error causes the file to
 * become inoperable. All function calls on inactive files do nothing, and mark
 * the file with an error of -1.
 */
bool file_active(std_file file);

#endif // STD_FILE_H
