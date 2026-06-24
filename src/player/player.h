#pragma once
#include "../camera/camera.h"
#include "../entity/entity.h"
#include "../events/event_sys.h"
#include "../utils/input.h"

namespace Engine {

class SceneManager;
class SpatialGrid;

class Player : public Entity {
public:
  Player(Camera &camera, InputManager &input, EventSystem *event);

  void update(float deltaTime);

  void setScene(SceneManager *scene) { m_scene = scene; }
  void setSpatialGrid(const SpatialGrid *grid) { m_spatialGrid = grid; }

  void setTeleportPending(const glm::vec3 &target);
  float getMoney() { return money; };
  void addPlrMoney(int val) { money = money + val; };
  void removePlrMoney(int val) { money = money - val; };
  bool isFlying() const { return m_flying; }
  bool isOnGround() const { return m_onGround; }
  const glm::vec3 &getVelocity() const { return m_velocity; }

private:
  Camera &m_camera;
  InputManager &m_input;
  EventSystem *m_eventsys;

  glm::vec3 m_velocity{0.0f};
  float money = 0.0f;

  bool m_flying = false;
  bool m_onGround = false;

  float m_eyeHeight = 1.7f;
  float m_playerHeight = 1.8f;
  float m_playerRadius = 0.3f;
  float m_gravity = -15.0f;
  float m_jumpForce = 7.0f;
  float m_walkSpeed = 5.0f;
  float m_sprintSpeed = 7.0f;
  float m_flySpeed = 10.0f;

  // bobbing
  float m_bobPhase = 0.0f;
  float m_bobOffset = 0.0f;
  float m_bobTilt = 0.0f;
  float m_bobVertical = 0.0f;
  float m_bobHorizontal = 0.0f;

  float m_bobFrequency = 1.0f;
  float m_bobVerticalAmplitude = 0.05f;
  float m_bobHorizontalAmplitude = 0.02f;
  float m_bobTiltAmplitude = 0.7f;
  float m_strafeTiltAmplitude = 0.6f;
  float m_bobTiltPhaseOffset = 1.5f;

  // landing shake
  bool m_wasOnGround = false;
  float m_shakeIntensity = 0.0f;
  float m_shakeTimer = 0.0f;
  float m_shakeDuration = 0.25f;
  float m_shakeVerticalOffset = 0.0f;
  float m_shakePitchOffset = 0.0f;
  float m_shakeYawOffset = 0.0f;
  float m_landingShakeStrength = 0.15f;

  // sprint FOV
  bool m_isSprinting = false;
  float m_sprintFov = 85.0f;
  float m_currentFov = 70.0f;
  float m_fovLerpSpeed = 6.0f;

  float speed = m_walkSpeed;

  SceneManager *m_scene = nullptr;
  const SpatialGrid *m_spatialGrid = nullptr;

  glm::vec3 m_pendingTeleport{0.0f};
  bool m_hasPendingTeleport;

  AABB getPlayerAABB(const glm::vec3 &pos) const;
  void resolveCollisionsAt(glm::vec3 &pos);
};

} // namespace Engine
