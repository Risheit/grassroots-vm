#include "io.h"
#include "error.h"
#include "memory.h"
#include "strings.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslimits.h>

typedef struct std_file {
  FILE *handle;
  int err;
  bool active;
} std_file;

#define RAW(file) (file).handle
#define ACTIVE(file)                                                           \
  do {                                                                         \
    if (!(file).handle) {                                                      \
      (file).err = errno;                                                      \
      (file).active = false;                                                   \
    }                                                                          \
    std_assert((file).active, #file " should be active");                      \
  } while (0)

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

std_file *file_open(std_arena *arena, std_string name, std_fopen_state state,
                    std_fopen_flags flags) {
  // Create a copy of [name] that is guaranteed to be null-terminated by
  // appending a null-terminator to the EOS.
  char buf[NAME_MAX + 1]; // Max path length buffer.
  std_arena *working_memory =
      arena_create_s(buf, (NAME_MAX + 1) * sizeof buf[0], 0);

  std_string safe_name = str_append(working_memory, name, str_null());

  std_file *file = arena_alloc(arena, sizeof(std_file));
  if (!file) {
    return nullptr;
  }

  file->handle = fopen(str_get(safe_name),
                       fopen_string(state, flags & FOPEN_NO_OVERWRITE));
  file->err = 0;
  file->active = true;

  // Failed to open file.
  if (!RAW(*file)) {
    file->err = errno;
    file->active = false;
  }

  return file;
}

std_file *file_temp(std_arena *arena) {
  std_file *file = arena_alloc(arena, sizeof(std_file));
  file->handle = tmpfile();
  file->err = 0;
  file->active = true;

  // Failed to open file.
  if (!RAW(*file)) {
    file->err = errno;
    file->active = false;
  }

  return file;
}

void file_close(std_file *file) {
  std_nonnull(file);
  ACTIVE(*file);

  int ret = fclose(file->handle);
  if (ret != 0) {
    file->err = errno;
  }
  file->active = false;
}

std_string file_read_line(std_arena *restrict arena, std_file *restrict file) {
  std_nonnull(arena);
  std_nonnull(file);
  ACTIVE(*file);

  size_t len;
  char *line = fgetln(file->handle, &len);

  // Failed to read line
  if (line == NULL) {
    if (feof(RAW(*file))) { // EOF reached
      file->err = FERR_EOF;
      return str_empty();
    }
    if (ferror(RAW(*file))) { // Error reached
      file->err = errno;
      return str_bad_ped(STERR_READ);
    }
  }

  return str_create(arena, line);
}

size_t file_write(const void *restrict ptr, size_t size, size_t n,
                  std_file *restrict file) {
  std_nonnull(ptr);
  std_nonnull(file);
  ACTIVE(*file);

  size_t write = fwrite(ptr, size, n, RAW(*file));
  if (write < n) {
    file->err = FERR_WRITE;
  }

  return write;
}

void file_flush(std_file *file) {
  std_nonnull(file);
  ACTIVE(*file);

  int res = fflush(RAW(*file));
  if (!res)
    file->err = errno;
}

long file_tell(std_file *file) {
  std_nonnull(file);
  ACTIVE(*file);

  long offset = ftell(RAW(*file));
  if (offset == -1)
    file->err = errno;

  return offset;
}

void file_seek(std_file *file, long offset, std_file_seek whence) {
  std_nonnull(file);
  ACTIVE(*file);

  int sys_whence; // System whence value based on stdio.h
  switch (whence) {
  case FSEEK_SET:
    sys_whence = SEEK_SET;
    break;
  case FSEEK_CUR:
    sys_whence = SEEK_CUR;
    break;
  case FSEEK_END:
    sys_whence = SEEK_END;
    break;
  }

  int res = fseek(RAW(*file), offset, sys_whence);
}

int file_err(const std_file *file) {
  std_nonnull(file);
  return file->err;
}

void file_reset_err(std_file *file) {
  std_nonnull(file);
  file->err = 0;
}

bool file_active(const std_file *file) {
  std_nonnull(file);
  return file->active;
}
