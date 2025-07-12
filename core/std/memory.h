#ifndef STD_MEMORY_H
#define STD_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Convenience type of the smallest possible representation of data. This is a
 * minimum of one byte.
 */
typedef uint_least8_t byte;

typedef enum std_arena_flags {
  ARENA_STOP_REALLOC = 1 << 0, // The arena's backing memory cannot be resized.
  CONTINUE_ON_ALLOC_FAILURE = 1 << 1, // Whether this arena should not panic if
                                      // backing memory fails to be allocated.
} std_arena_flags;

/**
 * Memory storage type. Arenas store large chunks of contiguous memory, and
 * allow for smaller allocations within. Memory allocated within an arena is
 * guaranteed to exist for the lifetime of that arena, blocks of memory cannot
 * be individually freed, though an arena can be cleaned for reuse using
 * [arena_clean].
 */
typedef struct std_arena std_arena;

/**
 * Initializes an arena [arena] of initial [size] bytes with [flags] flags. If
 * the allocation of an arena's backing memory or allocation of the arena itself
 * fails, then this function panics, unless [CONTINUE_ON_ALLOC_FAILURE] is
 * set, in which case it defines the arena's memory as unallocated, or
 * returns [NULL], respectively.
 */
std_arena *arena_create(size_t size, enum std_arena_flags flags);

/**
 * Initializes the arena [arena] with [size] bytes and [flags] flags, associated
 * with the backing memory [memory].
 *
 * When creating an arena this way, it is assumed that [memory] and the [arena]
 * pointer are managed externally to the arena structure, and reallocations and
 * automatic memory freeing is prevented. Consider using this function instead
 * of [arena_init] when providing a backing memory that is freed automatically,
 * such as stack-allocated memory.
 *
 * The returned arena pointer is also located in this memory, and is destroyed
 * when [memory] is destroyed.
 */
std_arena *arena_create_s(void *memory, size_t size,
                          enum std_arena_flags flags);

/**
 * Frees memory allocated by an arena and sets its [is_allocated] flag to
 * [false]. Accessing an arena pointer after calling [arena_delete] is undefined
 * behaviour.
 */
void arena_destroy(std_arena *arena);

/**
 * Allocate a pointer of [size] bytes within the arena. If allocation fails,
 * this function panics, unless [CONTINUE_ON_ALLOC_FAILURE] is set, in which
 * case it returns [NULL].
 * By default, if a pointer allocation would cause the arena to run out of
 * space, this function reallocates more space to the arena. If
 * [ARENA_DISALLOW_REALLOC] is set or the arena is allocated on the stack, then
 * if the arena would reallocate memory, it fails instead. If an arena memory
 * reallocation occurs and fails during this call and
 * [CONTINUE_ON_ALLOC_FAILURE] is set, the arena's [is_allocated] flag is
 * set to [false] and the function returns [NULL].
 */
void *arena_alloc(std_arena *arena, size_t size);

/**
 * De-allocates all memory initialized within an arena. Accessing any previous
 * memory allocated within an arena after calling [arena_clean] is undefined
 * behaviour.
 */
void arena_clean(std_arena *arena);

/**
 * Returns [true] if the backing memory for this arena is correctly allocated,
 * and [false] otherwise.
 */
bool is_allocated(std_arena *arena);

#endif // STD_MEMORY_H
