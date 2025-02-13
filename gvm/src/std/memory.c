#include "memory.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Arena allocator implementation details from:
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

arena arena_init(size_t size, enum __std_arena_flags flags) {
  byte *memory = malloc(size);

  arena arena = {
      .memory = memory,
      .size = size,
      .flags = flags,
  };

  if (memory != NULL)
    arena.iflags |= IS_ALLOCATED;

  return arena;
}

arena arena_init_s(byte *memory, size_t size, enum __std_arena_flags flags) {
  assert(memory != NULL);
  assert(size > 0);

  return (arena){.memory = memory,
                 .size = size,
                 .flags = flags,
                 .iflags = IS_ALLOCATED | IS_STACK};
}

void arena_delete(arena *arena) {
  arena->iflags &= ~IS_ALLOCATED;

  if (!(arena->iflags & IS_STACK)) {
    free(arena->memory);
  }
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

/**
 * Find offset to align [size] to word alignment.
 */
static size_t align_forward(size_t size) {
  const size_t alignment = 2 * sizeof(byte *);

  return alignment - (size % alignment);
}

void *arena_alloc(arena *arena, size_t size) {
  bool allow_realloc =
      !(arena->flags & ARENA_STOP_REALLOC) && !(arena->iflags & IS_STACK);
  byte *ptr;

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

  ptr = arena->memory + arena->offset;
  memset(ptr, 0, size);
  arena->offset += align_forward(size) + size;
  return ptr;
}

void arena_clean(arena *arena) { arena->offset = 0; }

bool is_allocated(arena *arena) { return arena->iflags & IS_ALLOCATED; }
