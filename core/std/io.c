#include "io.h"
#include "strings.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
  std_file file = {._handle = fopen(str_get(name), fopen_string(state)),
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
  if (!file._handle) {
    file._err = errno;
    file._active = false;
  }

  return file;
}

void file_close(std_file *file) {
  assert(file != NULL);

  if (!file->_active) {
    file->_err = -1;
    return;
  }

  int ret = fclose(file->_handle);
  if (ret != 0) {
    file->_err = errno;
  }
  file->_active = false;
}

int32_t file_err(const std_file *file) { return file->_err; }

bool file_active(const std_file *file) { return file->_active; }
