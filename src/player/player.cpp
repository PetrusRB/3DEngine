#include "player.h"
#include "../physics/spatial_grid.h"
#include "../renderer/scene.h"
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

namespace Engine {

Player::Player(Camera &camera, InputManager &input, EventSystem *event)
    : m_camera(camera), m_input(input), m_eventsys(event) {
  transform.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
}

void Player::setTeleportPending(const glm::vec3 &targetPos) {
  m_pendingTeleport = targetPos;
  m_hasPendingTeleport = true;
}
void Player::update(float deltaTime) {
  if (m_input.isKeyJustPressed(GLFW_KEY_V)) {
    m_flying = !m_flying;
    m_velocity = glm::vec3(0.0f);
    m_onGround = false;
  }

  m_input.updateMouseDelta();
  glm::dvec2 mouseDelta = m_input.getMouseDelta();
  if (mouseDelta.x != 0.0 || mouseDelta.y != 0.0)
    m_camera.processMouse(static_cast<float>(mouseDelta.x),
                          static_cast<float>(mouseDelta.y));

  glm::vec3 front = m_camera.getFront();
  glm::vec3 right = m_camera.getRight();

  if (m_flying) {
    glm::vec3 moveDir(0.0f);
    if (m_input.isKeyHeld(GLFW_KEY_W))
      moveDir += front;
    if (m_input.isKeyHeld(GLFW_KEY_S))
      moveDir -= front;
    if (m_input.isKeyHeld(GLFW_KEY_A))
      moveDir -= right;
    if (m_input.isKeyHeld(GLFW_KEY_D))
      moveDir += right;
    if (m_input.isKeyHeld(GLFW_KEY_SPACE))
      moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
    if (m_input.isKeyHeld(GLFW_KEY_LEFT_SHIFT))
      moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);

    if (glm::length(moveDir) > 0.001f)
      moveDir = glm::normalize(moveDir);
    m_velocity = moveDir * m_flySpeed;
  } else {
    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 moveDir(0.0f);
    if (m_input.isKeyHeld(GLFW_KEY_W))
      moveDir += flatFront;
    if (m_input.isKeyHeld(GLFW_KEY_S))
      moveDir -= flatFront;
    if (m_input.isKeyHeld(GLFW_KEY_A))
      moveDir -= right;
    if (m_input.isKeyHeld(GLFW_KEY_D))
      moveDir += right;

    if (glm::length(moveDir) > 0.001f)
      moveDir = glm::normalize(moveDir);
    // sprint FOV
    m_velocity.x = moveDir.x * speed;
    m_velocity.z = moveDir.z * speed;

    m_velocity.y += m_gravity * deltaTime;

    if (m_input.isKeyJustPressed(GLFW_KEY_SPACE) && m_onGround) {
      m_velocity.y = m_jumpForce;
      m_onGround = false;
    }
  }

  glm::vec3 pos = transform.getPosition();
  pos += m_velocity * deltaTime;

  for (int i = 0; i < 4; i++) {
    if (m_flying)
      break;
    glm::vec3 oldPos = pos;
    resolveCollisionsAt(pos);
    if (pos == oldPos)
      break;
  }
  if (m_hasPendingTeleport) {
    pos = m_pendingTeleport;
    m_hasPendingTeleport = false;
  }
  bool shouldBob = !m_flying && m_onGround &&
                   glm::length(glm::vec2(m_velocity.x, m_velocity.z)) > 0.1f;

  // landing shake detection
  if (m_onGround && !m_wasOnGround && m_shakeIntensity <= 0.0f) {
    float impactSpeed = -m_velocity.y;
    if (impactSpeed > 2.0f) {
      m_shakeIntensity =
          glm::clamp(impactSpeed * m_landingShakeStrength, 0.0f, 1.0f);
      m_shakeTimer = m_shakeDuration;
    }
  }
  m_wasOnGround = m_onGround;

  // landing shake update
  float shakeY = 0.0f;
  float shakePitch = 0.0f;
  float shakeYaw = 0.0f;
  if (m_shakeTimer > 0.0f) {
    m_shakeTimer -= deltaTime;
    float t = glm::clamp(m_shakeTimer / m_shakeDuration, 0.0f, 1.0f);
    float decay = t * t;
    float freq = 40.0f;
    shakeY = sin(m_shakeTimer * freq) * m_shakeIntensity * 0.03f * decay;
    shakePitch =
        sin(m_shakeTimer * freq * 1.3f) * m_shakeIntensity * 0.8f * decay;
    shakeYaw =
        cos(m_shakeTimer * freq * 0.7f) * m_shakeIntensity * 0.5f * decay;
  }

  float targetVertical = 0.0f;
  float targetHorizontal = 0.0f;
  float targetTilt = 0.0f;

  float strafeDot =
      glm::dot(glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)),
               glm::vec2(right.x, right.z));
  if (shouldBob) {
    float speed = glm::length(glm::vec2(m_velocity.x, m_velocity.z));
    m_bobPhase += speed * deltaTime * m_bobFrequency;

    targetVertical = sin(m_bobPhase) * m_bobVerticalAmplitude;
    targetHorizontal = cos(m_bobPhase) * m_bobHorizontalAmplitude;
    float stepTilt =
        sin(m_bobPhase + m_bobTiltPhaseOffset) * m_bobTiltAmplitude;

    float strafeTilt = strafeDot * m_strafeTiltAmplitude;
    targetTilt = stepTilt + strafeTilt;
  }

  float lerpSpeed = shouldBob ? 10.0f : 6.0f;
  m_bobVertical =
      glm::mix(m_bobVertical, targetVertical, lerpSpeed * deltaTime);
  m_bobHorizontal =
      glm::mix(m_bobHorizontal, targetHorizontal, lerpSpeed * deltaTime);
  m_bobTilt = glm::mix(m_bobTilt, targetTilt, lerpSpeed * deltaTime);

  m_isSprinting = !m_flying && m_input.isKeyHeld(GLFW_KEY_LEFT_SHIFT) &&
                  glm::length(glm::vec2(m_velocity.x, m_velocity.z)) > 0.1f;

  float targetSpeed = m_isSprinting ? m_sprintSpeed : m_walkSpeed;
  speed = glm::mix(speed, targetSpeed, 0.5f);

  float targetFov = m_isSprinting ? m_sprintFov : m_camera.getBaseFov();
  m_currentFov = glm::mix(m_currentFov, targetFov, m_fovLerpSpeed * deltaTime);
  m_camera.setFov(m_currentFov);

  if (!m_hasPendingTeleport) {
    transform.setPosition(pos);
  }
  m_camera.setPosition(pos + glm::vec3(m_bobHorizontal,
                                       m_eyeHeight + m_bobVertical + shakeY,
                                       0.0f));
  m_camera.setRoll(m_bobTilt + shakePitch);
}

AABB Player::getPlayerAABB(const glm::vec3 &pos) const {
  return {pos + glm::vec3(-m_playerRadius, 0.0f, -m_playerRadius),
          pos + glm::vec3(m_playerRadius, m_playerHeight, m_playerRadius)};
}

void Player::resolveCollisionsAt(glm::vec3 &pos) {
  if (!m_scene)
    return;

  AABB playerBox = getPlayerAABB(pos);

  if (m_spatialGrid) {
    std::vector<uint32_t> nearby;
    m_spatialGrid->query(playerBox, nearby);

    for (uint32_t id : nearby) {
      SceneObject *obj = m_scene->getObject(id);
      if (!obj || !obj->active)
        continue;

      glm::vec3 oPos = obj->transform.getPosition();
      glm::vec3 oScale = obj->transform.getScale();
      AABB objBox{oPos - oScale * 0.5f, oPos + oScale * 0.5f};

      if (!playerBox.overlaps(objBox))
        continue;

      if (m_eventsys)
        m_eventsys->emitCollision(obj->id, *this, *obj);

      glm::vec3 playerCenter = playerBox.getCenter();
      glm::vec3 objCenter = objBox.getCenter();

      float pushX = (playerCenter.x < objCenter.x)
                        ? objBox.min.x - playerBox.max.x
                        : objBox.max.x - playerBox.min.x;
      float pushY = (playerCenter.y < objCenter.y)
                        ? objBox.min.y - playerBox.max.y
                        : objBox.max.y - playerBox.min.y;
      float pushZ = (playerCenter.z < objCenter.z)
                        ? objBox.min.z - playerBox.max.z
                        : objBox.max.z - playerBox.min.z;

      float absPushX = std::abs(pushX);
      float absPushY = std::abs(pushY);
      float absPushZ = std::abs(pushZ);

      if (absPushX <= absPushY && absPushX <= absPushZ) {
        pos.x += pushX;
        m_velocity.x = 0.0f;
      } else if (absPushY <= absPushX && absPushY <= absPushZ) {
        pos.y += pushY;
        m_velocity.y = 0.0f;
        if (pushY > 0.0f)
          m_onGround = true;
      } else {
        pos.z += pushZ;
        m_velocity.z = 0.0f;
      }

      playerBox = getPlayerAABB(pos);
    }
  } else {
    for (auto &[id, obj] : m_scene->objects()) {
      if (!obj.active)
        continue;

      glm::vec3 oPos = obj.transform.getPosition();
      glm::vec3 oScale = obj.transform.getScale();
      AABB objBox{oPos - oScale * 0.5f, oPos + oScale * 0.5f};

      if (!playerBox.overlaps(objBox))
        continue;

      glm::vec3 playerCenter = playerBox.getCenter();
      glm::vec3 objCenter = objBox.getCenter();

      float pushX = (playerCenter.x < objCenter.x)
                        ? objBox.min.x - playerBox.max.x
                        : objBox.max.x - playerBox.min.x;
      float pushY = (playerCenter.y < objCenter.y)
                        ? objBox.min.y - playerBox.max.y
                        : objBox.max.y - playerBox.min.y;
      float pushZ = (playerCenter.z < objCenter.z)
                        ? objBox.min.z - playerBox.max.z
                        : objBox.max.z - playerBox.min.z;

      float absPushX = std::abs(pushX);
      float absPushY = std::abs(pushY);
      float absPushZ = std::abs(pushZ);

      if (absPushX <= absPushY && absPushX <= absPushZ) {
        pos.x += pushX;
        m_velocity.x = 0.0f;
      } else if (absPushY <= absPushX && absPushY <= absPushZ) {
        pos.y += pushY;
        m_velocity.y = 0.0f;
        if (pushY > 0.0f)
          m_onGround = true;
      } else {
        pos.z += pushZ;
        m_velocity.z = 0.0f;
      }

      playerBox = getPlayerAABB(pos);
    }
  }
}

} // namespace Engine
