#include "memory.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Arena allocator implementation details from:
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

struct arena {
  size_t size;            // The arena size.
  size_t offset;          // Current offset in memory.
  void *memory;           // Backing memory.
  enum arena_flags flags; // Set arena flags.
  bool is_allocated;      // If this arena has valid backing memory.
};

struct arena *arena_init(size_t size, enum arena_flags flags) {
  struct arena *arena = malloc(sizeof(*arena));
  void *memory;

  if (arena == NULL)
    return NULL;

  memory = malloc(size);
  arena->memory = memory;
  arena->offset = 0;
  arena->size = size;
  arena->flags = flags;
  arena->is_allocated = (memory != NULL);

  return arena;
}

void arena_delete(struct arena *arena) {
  free(arena->memory);
  arena->is_allocated = false;
  free(arena);
}

static void reallocate(struct arena *arena) {
  void *realloc_mem = realloc(arena->memory, arena->size * 2);
  arena->memory = realloc_mem;
  arena->size = arena->size * 2;
  arena->is_allocated = (realloc_mem != NULL);
}

/**
 * Find offset to align [size] to word alignment.
 */
static size_t align_forward(size_t size) {
  const size_t alignment = 2 * sizeof(void *);

  return alignment - (size % alignment);
}

void *arena_alloc(struct arena *arena, size_t size) {
  bool allow_realloc = (arena->flags & ARENA_DISALLOW_REALLOC);
  void *ptr;

  assert(arena->is_allocated);
  assert(size > 0);

  if (arena->size < size + arena->offset) {
    if (allow_realloc)
      reallocate(arena);
    else
      return NULL;

    if (arena->is_allocated == false) // Reallocation failed
      return NULL;
  }

  ptr = arena->memory + arena->offset;
  memset(ptr, 0, size);
  arena->offset += align_forward(size) + size;
  return ptr;
}

void arena_clean(struct arena *arena) { arena->offset = 0; }

bool is_allocated(struct arena *arena) { return arena->is_allocated; }
