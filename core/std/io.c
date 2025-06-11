#include "io.h"
#include "memory.h"
#include "strings.h"
#include <_stdio.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/syslimits.h>

#define RAW(file) (file)._handle

static const char *fopen_string(std_fopen_state state) {
  switch (state) {
  case FOPEN_READ:
    return "r";
  case FOPEN_READPL:
    return "r+";
  case FOPEN_WRITE:
    return "w";
  case FOPEN_WRITEPL:
    return "w+";
  case FOPEN_APPEND:
    return "a";
  case FOPEN_APPENDPL:
    return "a+";
  }
}

std_file file_open(std_string name, std_fopen_state state) {
  // Create a copy of [name] that is guaranteed to be null-terminated by
  // appending a null-terminator to the EOS.
  char buf[NAME_MAX + 1]; // Max path length buffer.
  std_arena *arena = arena_create_s(buf, (NAME_MAX + 1) * sizeof buf, 0);
  std_string safe_name = str_append(arena, name, str_null());

  std_file file = {._handle = fopen(str_get(safe_name), fopen_string(state)),
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
  assert(file != NULL);

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

std_string file_read_line(std_arena *arena, std_file *file) {
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
  }

  return str_create(arena, line);
}

int32_t file_err(const std_file *file) { return file->_err; }

bool file_active(const std_file *file) { return file->_active; }
