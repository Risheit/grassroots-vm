#ifndef STD_IO_H
#define STD_IO_H

#include "memory.h"
#include "strings.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * FILE pointer wrapper. Elements should be accessed using accessor functions
 * rather than directly.
 */
typedef struct std_file {
  FILE *_handle;
  int _err;
  bool _active;
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

typedef enum std_fopen_flags {
  FOPEN_NO_OVERWRITE = 1, // Equivalent to adding "x" to a "w" or "w+" call.
} std_fopen_flags;

enum {
  FERR_INACTIVE = -1, // Operation performed on inactive file.
  FERR_EOF = -2,      // Operation performed on EOF.
  FERR_WRITE = -3,    // Partially or completely failed a write operation.
};

/**
 * Opens a file. On a failure, errno is set as specified by [fopen], and the
 * file returned is inactive and marked with the relevant error number.
 */
std_file file_open(std_string name, std_fopen_state state,
                   std_fopen_flags flags);

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
 * occurs, and [file] is marked with [FERR_INACTIVE].
 */
void file_close(std_file *file);

/**
 * Reads up to and including a newline character or EOF in [file]. On a failure,
 * an error string is returned, errno is set as specified by [fgetln], and
 * [file] is marked with the relevant error number. The string containing the
 * read line is returned, allocated in [arena]. Note that if the line is the
 * last in a file that does not end in a newline, the returned text will not
 * contain a newline (in accordance with [fgetln]. On a read of EOF, an empty
 * string is returned, and [file] is marked with [FERR_EOF].
 */
std_string file_read_line(std_arena *restrict arena, std_file *restrict file);

/**
 * Writes [n] items to [file], each of size [size] from memory [ptr], with
 * semantics equivalent to [fwrite]. If an error occurs and [fwrite] writes
 * less than [n] items, then [file] is marked with [FERR_WRITE].
 */
size_t file_write(const void *restrict ptr, size_t size, size_t n,
                  std_file *restrict file);

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
int file_err(const std_file *file);

/**
 * Returns true if [file] is active, and false if not. Files are marked
 * inactive after they are closed, or if a critical error causes the file to
 * become inoperable. All function calls on inactive files do nothing, and mark
 * the file with an error of [FERR_INACTIVE].
 */
bool file_active(const std_file *file);

#endif // STD_IO_H
