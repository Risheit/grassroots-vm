#ifndef __STD_MEMORY_H
#define __STD_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum arena_flags {
  ARENA_DISALLOW_REALLOC = 1 // The arena's backing memory cannot be resized.
};

/**
 * Memory storage type. Arenas store large chunks of contiguous memory, and
 * allow for smaller allocations within. Memory allocated within an arena is
 * guaranteed to exist for the lifetime of that arena, blocks of memory cannot
 * be individually freed, though an arena can be cleaned for reuse using
 * [arena_clean].
 */
struct arena;

/**
 * Initializes an arena of [size] bytes with [flags] flags. If the allocation
 * of an arena's backing memory fails, the arena's [is_allocated] flag is set to
 * [false]. If the allocation of the arena itself fails, then the returned
 * pointer is [NULL].
 */
struct arena *arena_init(size_t size, enum arena_flags flags);

/**
 * Frees memory allocated by an arena and sets its [is_allocated] flag to
 * [false]. Accessing an arena pointer after calling [arena_delete] is undefined
 * behaviour.
 */
void arena_delete(struct arena *arena);

/**
 * Allocate a pointer of [size] bytes within the arena. If allocation fails,
 * returns [NULL]. If an arena memory reallocation occurs and fails during this
 * call, the arena's [is_allocated] flag is set to [false] and the function
 * returns [NULL].
 * If [ARENA_DISALLOW_REALLOC] is set, then if the arena would reallocate
 * memory, return NULL instead.
 */
void *arena_alloc(struct arena *arena, size_t size);

/**
 * De-allocates all memory initialized within an arena. Accessing any previous
 * memory allocated within an arena after calling [arena_clean] is undefined
 * behaviour.
 */
void arena_clean(struct arena *arena);

/**
 * Returns [true] if the backing memory for this arena is correctly allocated,
 * and [false] otherwise.
 * This should be run after an [arena_init] to ensure that the backing memory is
 * initialized correctly.
 */
bool is_allocated(struct arena *arena);

#endif // __STD_MEMORY_H
