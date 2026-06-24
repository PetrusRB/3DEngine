#pragma once
#include "arena_wrapper.h"
#include <cstddef>
#include <cstdint>

namespace Engine {

template <typename T> class ArenaAllocator {
public:
  using value_type = T;

  ArenaAllocator() noexcept : m_arena(nullptr) {}
  explicit ArenaAllocator(t_arena *arena) noexcept : m_arena(arena) {}

  template <typename U>
  ArenaAllocator(const ArenaAllocator<U> &other) noexcept
      : m_arena(other.getArena()) {}

  T *allocate(size_t n) {
    if (n > SIZE_MAX / sizeof(T))
      return nullptr;
    void *ptr = arena_wrap_alloc(m_arena, n * sizeof(T));
    if (!ptr)
      return nullptr;
    return static_cast<T *>(ptr);
  }

  void deallocate(T *, size_t) noexcept {}

  t_arena *getArena() const { return m_arena; }

private:
  t_arena *m_arena;
};

template <typename T, typename U>
bool operator==(const ArenaAllocator<T> &a, const ArenaAllocator<U> &b) {
  return a.getArena() == b.getArena();
}

template <typename T, typename U>
bool operator!=(const ArenaAllocator<T> &a, const ArenaAllocator<U> &b) {
  return !(a == b);
}

} // namespace Engine
