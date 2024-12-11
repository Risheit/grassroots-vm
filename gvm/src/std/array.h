#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define vptr(ptr) (void *)(ptr)
#define vref(ptr) (void **)(ptr)

/**
 * Metadata that corresponds to a specific array.
 *
 */
struct _std_arr_mt {
  uint64_t len;
  uint64_t elmt_size;
  bool valid;
};
typedef struct _std_arr_mt std_arrmt_t;

/**
 * Allocates memory for an array of [len] elements each with [size] element
 * size, and stores it in the [arr] pointer. Metadata information corresponding
 * to this array is returned.
 *
 * If the array fails to be constructed, the [valid] field within the returned
 * metadata is [false].
 */
std_arrmt_t std_arr_construct(void **arr, uint64_t len, uint64_t size);

/**
 * Destroys the given array, setting the pointer to it and freeing any allocated
 * memory. If the metadata field is provided, the [valid] field in the provided
 * metadata [mt] is set to false.
 */
void std_arr_destroy(void **arr, std_arrmt_t *mt);

/**
 * Extends the given array by [ext] elements. This replaces the given [arr]
 * pointer, and frees the memory previously allocated to the array, invalidating
 * any other pointers to this array.
 *
 * If an error occurs, this function returns 0 and does not invalidate the
 * provided array.
 */
uint8_t std_arr_extend(void **arr, std_arrmt_t *mt, uint64_t ext);
