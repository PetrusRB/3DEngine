#pragma once
#include <stddef.h>

typedef struct s_arena t_arena;

#ifdef __cplusplus
extern "C" {
#endif

t_arena *arena_wrap_create(size_t size);
void     arena_wrap_destroy(t_arena *arena);
void     arena_wrap_reset(t_arena *arena);
void    *arena_wrap_alloc(t_arena *arena, size_t size);

#ifdef __cplusplus
}
#endif
