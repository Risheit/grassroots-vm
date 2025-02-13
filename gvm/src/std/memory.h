#ifndef __STD_MEMORY_H
#define __STD_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Standard memory storage type. Is not always a byte of memory.
 */
typedef unsigned char byte;

enum __std_arena_flags {
  ARENA_STOP_REALLOC = 1 << 0, // The arena's backing memory cannot be resized.
};

typedef enum __std_arena_flags arena_flags;

enum __std_arena_internal_flags {
  IS_ALLOCATED = 1 << 0, // Whether this arena's backing memory is valid.
  IS_STACK = 1 << 1,     // Whether this arena's backing memory is on the stack.
};

struct __std_arena {
  size_t size;                            // The arena size.
  size_t offset;                          // Current offset in memory.
  byte *memory;                           // Backing memory.
  arena_flags flags;                      // Set arena flags.
  enum __std_arena_internal_flags iflags; // Set internal arena flags.
};

/**
 * Memory storage type. Arenas store large chunks of contiguous memory, and
 * allow for smaller allocations within. Memory allocated within an arena is
 * guaranteed to exist for the lifetime of that arena, blocks of memory cannot
 * be individually freed, though an arena can be cleaned for reuse using
 * [arena_clean].
 */
typedef struct __std_arena arena;

/**
 * Initializes an arena [arena] of [size] bytes with [flags] flags. If the
 * allocation of an arena's backing memory fails, the arena's [is_allocated]
 * flag is set to [false]. If the allocation of the arena itself fails, then the
 * returned pointer is [NULL].
 */
arena arena_init(size_t size, enum __std_arena_flags flags);

#define new_arena(size, flags)

/**
 * Initializes the arena [arena] with [size] bytes and [flags] flags, associated
 * with the backing memory [memory].
 *
 * When creating an arena this way, it is assumed that [memory] and the [arena]
 * pointer are managed externally to the arena structure, and reallocations and
 * automatic memory freeing is prevented. Consider using this function instead
 * of [arena_init] when providing a backing memory and [arena] pointer that is
 * freed automatically, such as stack-allocated memory.
 */
arena arena_init_s(byte *memory, size_t size, enum __std_arena_flags flags);

/**
 * Frees memory allocated by an arena and sets its [is_allocated] flag to
 * [false]. Accessing an arena pointer after calling [arena_delete] is undefined
 * behaviour.
 */
void arena_delete(arena *arena);

/**
 * Allocate a pointer of [size] bytes within the arena. If allocation fails,
 * returns [NULL].
 * By default, if a pointer allocation would cause the arena to run out of
 * space, this function reallocates more space to the arena. If
 * [ARENA_DISALLOW_REALLOC] is set or the arena is allocated on the stack, then
 * if the arena would reallocate memory, return NULL instead.
 * If an arena memory reallocation occurs and fails during this call, the
 * arena's [is_allocated] flag is set to [false] and the function returns
 * [NULL].
 */
void *arena_alloc(arena *arena, size_t size);

/**
 * De-allocates all memory initialized within an arena. Accessing any previous
 * memory allocated within an arena after calling [arena_clean] is undefined
 * behaviour.
 */
void arena_clean(arena *arena);

/**
 * Returns [true] if the backing memory for this arena is correctly allocated,
 * and [false] otherwise.
 * This should be run after an [arena_init] to ensure that the backing memory is
 * initialized correctly.
 */
bool is_allocated(arena *arena);

#endif // __STD_MEMORY_H
