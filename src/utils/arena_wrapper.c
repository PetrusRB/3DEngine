#include "arena_wrapper.h"
#include <arena/arena.h>

t_arena *arena_wrap_create(size_t size) {
  return arena_create(size, false);
}

void arena_wrap_destroy(t_arena *arena) { arena_destroy(arena); }

void arena_wrap_reset(t_arena *arena) { arena_reset(arena); }

void *arena_wrap_alloc(t_arena *arena, size_t size) {
  return arena_alloc(arena, size);
}
