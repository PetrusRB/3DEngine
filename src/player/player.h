#pragma once
#include "../camera/camera.h"
#include "../entity/entity.h"
#include "../utils/input.h"

namespace Engine {

class SceneManager;
class SpatialGrid;

class Player : public Entity {
public:
  Player(Camera &camera, InputManager &input);

  void update(float deltaTime);

  void setScene(SceneManager *scene) { m_scene = scene; }
  void setSpatialGrid(const SpatialGrid *grid) { m_spatialGrid = grid; }

  bool isFlying() const { return m_flying; }
  bool isOnGround() const { return m_onGround; }
  const glm::vec3 &getVelocity() const { return m_velocity; }

private:
  Camera &m_camera;
  InputManager &m_input;

  glm::vec3 m_velocity{0.0f};
  bool m_flying = false;
  bool m_onGround = false;

  float m_eyeHeight = 1.7f;
  float m_playerHeight = 1.8f;
  float m_playerRadius = 0.3f;
  float m_gravity = -15.0f;
  float m_jumpForce = 7.0f;
  float m_moveSpeed = 5.0f;
  float m_flySpeed = 10.0f;

  SceneManager *m_scene = nullptr;
  const SpatialGrid *m_spatialGrid = nullptr;

  AABB getPlayerAABB(const glm::vec3 &pos) const;
  void resolveCollisionsAt(glm::vec3 &pos);
};

} // namespace Engine
