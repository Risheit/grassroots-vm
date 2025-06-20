#include "io.h"
#include "error.h"
#include "memory.h"
#include "strings.h"
#include <_stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/syslimits.h>

#define RAW(file) (file)._handle
#define RESET_ERR(file) (file)._err = 0

static const char *fopen_string(std_fopen_state state, bool dont_overwrite) {
  switch (state) {
  case FOPEN_READ:
    return "r";
  case FOPEN_READPL:
    return "r+";
  case FOPEN_WRITE:
    return dont_overwrite ? "wx" : "w";
  case FOPEN_WRITEPL:
    return dont_overwrite ? "wx+" : "w+";
  case FOPEN_APPEND:
    return "a";
  case FOPEN_APPENDPL:
    return "a+";
  }
}

std_file file_open(std_string name, std_fopen_state state,
                   std_fopen_flags flags) {
  // Create a copy of [name] that is guaranteed to be null-terminated by
  // appending a null-terminator to the EOS.
  char buf[NAME_MAX + 1]; // Max path length buffer.
  std_arena *arena = arena_create_s(buf, (NAME_MAX + 1) * sizeof buf[0], 0);
  std_string safe_name = str_append(arena, name, str_null());

  std_file file = {._handle =
                       fopen(str_get(safe_name),
                             fopen_string(state, flags & FOPEN_NO_OVERWRITE)),
                   ._err = 0,
                   ._active = true};

  // Failed to open file.
  if (!file._handle) {
    file._err = errno;
    file._active = false;
  }

  return file;
}

std_file file_temp() {
  std_file file = {._handle = tmpfile(), ._err = 0, ._active = true};

  // Failed to open file.
  if (!RAW(file)) {
    file._err = errno;
    file._active = false;
  }

  return file;
}

void file_close(std_file *file) {
  std_nonnull(file);
  RESET_ERR(*file);

  // Action on inactive file
  if (!file->_active) {
    file->_err = FERR_INACTIVE;
    return;
  }

  int ret = fclose(file->_handle);
  if (ret != 0) {
    file->_err = errno;
  }
  file->_active = false;
}

std_string file_read_line(std_arena *restrict arena, std_file *restrict file) {
  std_nonnull(arena);
  std_nonnull(file);
  RESET_ERR(*file);

  size_t len;
  char *line = fgetln(file->_handle, &len);

  // Failed to read line
  if (line == NULL) {
    if (feof(RAW(*file))) { // EOF reached
      file->_err = FERR_EOF;
      return str_empty();
    }
    if (ferror(RAW(*file))) {
      file->_err = errno;
      return str_bad_ped(STERR_READ);
    }

    file->_err = 0;
  }

  return str_create(arena, line);
}

size_t file_write(const void *restrict ptr, size_t size, size_t n,
                  std_file *restrict file) {
  std_nonnull(ptr);
  std_nonnull(file);
  RESET_ERR(*file);

  size_t write = fwrite(ptr, size, n, RAW(*file));
  if (write < n) {
    file->_err = FERR_WRITE;
  } 

  return write;
}

int file_err(const std_file *file) {
  std_nonnull(file);
  return file->_err;
}

bool file_active(const std_file *file) {
  std_nonnull(file);
  return file->_active;
}
