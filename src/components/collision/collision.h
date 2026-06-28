#pragma once
#include "../component.h"
#include "../../entity/entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

enum class CollisionType { Box, Sphere, Cylinder };

class Collision : public Component {
public:
  Collision() = default;
  virtual ~Collision() = default;

  virtual CollisionType getType() const = 0;
  virtual AABB getLocalAABB() const = 0;

  AABB getWorldAABB(const Transform &entityTransform) const;

  void setPosition(const glm::vec3 &pos);
  void setRotation(const glm::vec3 &rot);
  void setScale(const glm::vec3 &scl);

  const glm::vec3 &getPosition() const { return m_position; }
  const glm::vec3 &getRotation() const { return m_rotation; }
  const glm::vec3 &getScale() const { return m_scale; }

  bool isTrigger() const { return m_isTrigger; }
  bool isActive() const { return m_isActive; }
  void setTrigger(bool trigger) { m_isTrigger = trigger; }
  void setActive(bool active) { m_isActive = active; }

  void renderUI() override;
  const char *name() const override { return "Collision"; }

  const glm::mat4 &getOffsetMatrix() const;

  glm::vec3 m_position{0.0f};
  glm::vec3 m_rotation{0.0f};
  glm::vec3 m_scale{1.0f};
  bool m_isTrigger{false};
  bool m_isActive{true};

private:
  mutable glm::mat4 m_offsetMatrix{1.0f};
  mutable bool m_offsetDirty{true};
};

} // namespace Engine
