#include "array.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

std_arrmt_t std_arr_construct(void **arr, uint64_t len, uint64_t size) {
  assert(arr);
  assert(*arr);

  *arr = calloc(len, size);
  std_arrmt_t mt = {.len = len, .elmt_size = size, .valid = (arr)};
  return mt;
}

void std_arr_destroy(void **arr, std_arrmt_t *mt) {
  assert(arr);
  assert(*arr);

  free(*arr);
  *arr = NULL;
  if (mt)
    mt->valid = false;
}

uint8_t std_arr_extend(void **arr, std_arrmt_t *mt, uint64_t ext) {
  assert(arr);
  assert(*arr);
  assert(mt);

  if (ext == 0) // Nothing to do!
    return 1;

  if (mt->len + ext < mt->len) // Integer overflow
    return 0;

  void *new = calloc(mt->len + ext, mt->elmt_size);

  if (!new)
    return 0;

  *arr = new;
  mt->len += ext;
  return 1;
}
