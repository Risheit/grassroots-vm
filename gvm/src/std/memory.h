#ifndef STD_MEMORY_H
#define STD_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Standard memory storage type. Is not always a byte of memory.
 */
typedef unsigned char byte;

enum std_arena_flags {
  ARENA_STOP_REALLOC = 1 << 0, // The arena's backing memory cannot be resized.
};

typedef enum std_arena_flags arena_flags;

/**
 * Memory storage type. Arenas store large chunks of contiguous memory, and
 * allow for smaller allocations within. Memory allocated within an arena is
 * guaranteed to exist for the lifetime of that arena, blocks of memory cannot
 * be individually freed, though an arena can be cleaned for reuse using
 * [arena_clean].
 */
typedef struct std_arena arena;

/**
 * Initializes an arena [arena] of [size] bytes with [flags] flags. If the
 * allocation of an arena's backing memory fails, the arena's [is_allocated]
 * flag is set to [false]. If the allocation of the arena itself fails, then the
 * returned pointer is [NULL].
 */
arena *arena_create(size_t size, arena_flags flags);

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
 *
 * The returned arena pointer is also located in this memory, and is destroyed
 * when [memory] is destroyed.
 */
arena *arena_create_s(void *memory, size_t size, arena_flags flags);

/**
 * Frees memory allocated by an arena and sets its [is_allocated] flag to
 * [false]. Accessing an arena pointer after calling [arena_delete] is undefined
 * behaviour.
 */
void arena_destroy(arena *arena);

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
inline void arena_clean(arena *arena) { arena->offset = 0; }

/**
 * Returns [true] if the backing memory for this arena is correctly allocated,
 * and [false] otherwise.
 * This should be run after an [arena_init] to ensure that the backing memory is
 * initialized correctly.
 */
inline bool is_allocated(arena *arena) { return arena->iflags & IS_ALLOCATED; }

#endif // STD_MEMORY_H
