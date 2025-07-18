#include "memory.h"
#include "error.h"
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
  enum std_arena_flags flags; // Set arena flags.
  enum internal_flags iflags; // Set internal arena flags.
  byte *memory;               // Backing memory.
};

std_arena *arena_create(size_t size, enum std_arena_flags flags) {
  std_arena *arena = calloc(1, sizeof *arena);

  if (arena == NULL) {
    if (!(flags & CONTINUE_ON_ALLOC_FAILURE))
      std_panic("Failed to allocate memory for arena object");
    return nullptr;
  }

  byte *memory = malloc(size);

  arena->memory = memory;
  arena->size = size;
  arena->flags = flags;

  if (memory != NULL) {
    arena->flags |= IS_ALLOCATED;
  } else {
    if (!(flags & CONTINUE_ON_ALLOC_FAILURE))
      std_panic("Failed to allocate arena's backing memory");
  }

  return arena;
}

/**
 * Find offset to align [size] to word alignment.
 */
static size_t align_forward(size_t size) {
  const size_t alignment = 2 * sizeof(byte *);

  return alignment - (size % alignment);
}

std_arena *arena_create_s(void *memory, size_t size, std_arena_flags flags) {
  std_arena *arena = memory;

  std_nonnull(memory);
  std_assert(
      size > sizeof *arena,
      "arena size must be able to contain an arena object (at least %zu bytes)",
      sizeof *arena);

  memset(arena, 0, sizeof *arena);
  arena->memory = memory;
  // Adjust offset in [memory] to account for [arena] allocation.
  arena->offset = align_forward(sizeof(*arena)) + sizeof(*arena);
  arena->flags = flags;
  arena->size = size;
  arena->iflags = IS_ALLOCATED | IS_STACK;

  return arena;
}

void arena_destroy(std_arena *arena) {
  arena->iflags &= ~IS_ALLOCATED;

  if (!(arena->iflags & IS_STACK)) {
    free(arena->memory);
    free(arena);
  }
  *arena = (struct std_arena){0};
}

static void reallocate(std_arena *arena, size_t min_realloc) {
  size_t realloc_amt = arena->size != 0 ? arena->size * 2 : 1;

  while (realloc_amt <= min_realloc)
    realloc_amt *= 2;

  byte *realloc_mem = realloc(arena->memory, realloc_amt);
  arena->memory = realloc_mem;
  arena->size = arena->size * 2;
  arena->iflags &= ~IS_ALLOCATED;
  if (realloc_mem != NULL)
    arena->iflags |= IS_ALLOCATED;
}

void *arena_alloc(std_arena arena[static 1], size_t size) {
  std_assert(is_allocated(arena), "arena memory must exist");

  if (arena->size < size + arena->offset) {
    bool allow_realloc =
        !(arena->flags & ARENA_STOP_REALLOC) && !(arena->iflags & IS_STACK);

    if (allow_realloc)
      reallocate(arena, size + arena->offset);
    else {
      if (!(arena->flags & CONTINUE_ON_ALLOC_FAILURE))
        std_panic("Failed to allocate %zu bytes", size);
      return nullptr;
    }

    if (!is_allocated(arena)) // Reallocation failed
    {
      if (!(arena->flags & CONTINUE_ON_ALLOC_FAILURE))
        std_panic("Failed to reallocate backing memory.");
      return nullptr;
    }
  }

  byte *ptr = arena->memory + arena->offset;
  memset(ptr, 0, size);
  arena->offset += align_forward(size) + size;
  return ptr;
}

void arena_clean(std_arena *arena) { arena->offset = 0; }

bool is_allocated(std_arena *arena) { return arena->iflags & IS_ALLOCATED; }
