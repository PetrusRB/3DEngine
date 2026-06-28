#pragma once
#include "../collision.h"

namespace Engine {

class SphereCollider : public Collision {
public:
  SphereCollider() = default;
  SphereCollider(float radius);

  CollisionType getType() const override { return CollisionType::Sphere; }
  AABB getLocalAABB() const override;

  void setRadius(float r) { m_radius = r; }
  float getRadius() const { return m_radius; }

  void renderUI() override;
  const char *name() const override { return "SphereCollider"; }

  float m_radius{0.5f};
};

} // namespace Engine
