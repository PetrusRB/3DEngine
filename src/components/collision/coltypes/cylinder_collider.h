#pragma once
#include "../collision.h"

namespace Engine {

enum class CylinderAxis { X, Y, Z };

class CylinderCollider : public Collision {
public:
  CylinderCollider() = default;
  CylinderCollider(float radius, float height,
                   CylinderAxis axis = CylinderAxis::Y);

  CollisionType getType() const override { return CollisionType::Cylinder; }
  AABB getLocalAABB() const override;

  void setRadius(float r) { m_radius = r; }
  void setHeight(float h) { m_height = h; }
  void setAxis(CylinderAxis a) { m_axis = a; }

  float getRadius() const { return m_radius; }
  float getHeight() const { return m_height; }
  CylinderAxis getAxis() const { return m_axis; }

  void renderUI() override;
  const char *name() const override { return "CylinderCollider"; }

  float m_radius{0.5f};
  float m_height{1.0f};
  CylinderAxis m_axis{CylinderAxis::Y};
};

} // namespace Engine
