#include "memory.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Arena allocator implementation details from:
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

enum internal_flags {
  IS_ALLOCATED = 1 << 0, // Whether this arena's backing memory is valid.
  IS_STACK = 1 << 1,     // Whether this arena's backing memory is on the stack.
};

struct std_arena {
  size_t size;                // The arena size.
  size_t offset;              // Current offset in memory.
  arena_flags flags;          // Set arena flags.
  enum internal_flags iflags; // Set internal arena flags.
  byte *memory;               // Backing memory.
};

arena *arena_create(size_t size, enum std_arena_flags flags) {
  arena *arena = calloc(1, sizeof(*arena));

  if (arena == NULL)
    return NULL;

  byte *memory = malloc(size);

  arena->memory = memory;
  arena->size = size;
  arena->flags = flags;

  if (memory != NULL)
    arena->iflags |= IS_ALLOCATED;

  return arena;
}

/**
 * Find offset to align [size] to word alignment.
 */
static size_t align_forward(size_t size) {
  const size_t alignment = 2 * sizeof(byte *);

  return alignment - (size % alignment);
}

arena *arena_create_s(void *memory, size_t size, arena_flags flags) {
  arena *arena = memory;

  assert(memory != NULL);
  assert(size > sizeof(*arena));

  memset(arena, 0, sizeof(*arena));
  arena->memory = memory;
  // Adjust offset in [memory] to account for [arena] allocation.
  arena->offset = align_forward(sizeof(*arena)) + sizeof(*arena);
  arena->flags = flags;
  arena->iflags = IS_ALLOCATED | IS_STACK;

  return arena;
}

void arena_destroy(arena *arena) {
  arena->iflags &= ~IS_ALLOCATED;

  if (!(arena->iflags & IS_STACK)) {
    free(arena->memory);
    free(arena);
  }
  *arena = (struct std_arena){0};
}

static void reallocate(arena *arena, size_t min_realloc) {
  size_t realloc_amt = arena->size * 2;

  while (realloc_amt <= min_realloc)
    realloc_amt *= 2;

  byte *realloc_mem = realloc(arena->memory, realloc_amt);
  arena->memory = realloc_mem;
  arena->size = arena->size * 2;
  arena->iflags &= ~IS_ALLOCATED;
  if (realloc_mem != NULL)
    arena->iflags |= IS_ALLOCATED;
}

void *arena_alloc(arena arena[static 1], size_t size) {
  bool allow_realloc =
      !(arena->flags & ARENA_STOP_REALLOC) && !(arena->iflags & IS_STACK);

  assert(is_allocated(arena));
  assert(size > 0);

  if (arena->size < size + arena->offset) {
    if (allow_realloc)
      reallocate(arena, size + arena->offset);
    else
      return NULL;

    if (!is_allocated(arena)) // Reallocation failed
      return NULL;
  }

  byte *ptr = arena->memory + arena->offset;
  memset(ptr, 0, size);
  arena->offset += align_forward(size) + size;
  return ptr;
}

void arena_clean(arena *arena) { arena->offset = 0; }

bool is_allocated(arena *arena) { return arena->iflags & IS_ALLOCATED; }
