#include "file.h"
#include "strings.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct file_data {
  FILE *handle;
  int32_t err;
  bool active;
} file_data;

#define RAW(handle) ((file_data *)(handle).raw)[0]
#define SET_FILE(file, ptr)                                                    \
  memcpy(file.raw, &(file_data){.handle = ptr, .err = 0, .active = true},      \
         _STD_FILE_RAW_SIZE)

static_assert(sizeof(std_file) >= sizeof(file_data),
              "String handle is smaller than data type. Increase the "
              "string handle size in the strings.h header file.");
static_assert(alignof(std_file) == alignof(file_data),
              "String handle alignment is different from string type "
              "alignment. Modify the string alignment in strings.h");

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
  std_file file;
  SET_FILE(file, fopen(str_get(name), fopen_string(state)));

  // Failed to open file.
  if (!RAW(file).handle) {
    RAW(file).err = errno;
    RAW(file).active = false;
  }

  return file;
}

std_file file_temp() {
  std_file file;
  SET_FILE(file, tmpfile());

  // Failed to open file.
  if (!RAW(file).handle) {
    RAW(file).err = errno;
    RAW(file).active = false;
  }

  return file;
}

void file_close(std_file *file) {
  assert(file != NULL);

  if (!RAW(*file).active) {
    RAW(*file).err = -1;
    return;
  }

  int ret = fclose(RAW(*file).handle);
  if (ret != 0) {
    RAW(*file).err = errno;
  }
  RAW(*file).active = false;
}

int32_t file_err(const std_file *file) { return RAW(*file).err; }

bool file_active(const std_file *file) { return RAW(*file).active; }
