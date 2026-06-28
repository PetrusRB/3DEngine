#pragma once
#include "../components/transform/transform.h"
#include <algorithm>
#include <glm/glm.hpp>

namespace Engine {

struct AABB {
  glm::vec3 min{0.0f};
  glm::vec3 max{0.0f};

  bool overlaps(const AABB &other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
  }

  glm::vec3 getCenter() const { return (min + max) * 0.5f; }
};

class Entity {
public:
  Entity() = default;
  virtual ~Entity() = default;

  Transform transform;
};

} // namespace Engine
