#pragma once
#include "../collision.h"

namespace Engine {

class BoxCollider : public Collision {
public:
  BoxCollider() = default;
  BoxCollider(const glm::vec3 &halfExtents);

  CollisionType getType() const override { return CollisionType::Box; }
  AABB getLocalAABB() const override;

  void setHalfExtents(const glm::vec3 &he) { m_halfExtents = he; }
  const glm::vec3 &getHalfExtents() const { return m_halfExtents; }

  void renderUI() override;
  const char *name() const override { return "BoxCollider"; }

  glm::vec3 m_halfExtents{0.5f};
};

} // namespace Engine
