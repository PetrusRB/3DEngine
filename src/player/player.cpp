#include "player.h"
#include "../physics/spatial_grid.h"
#include "../renderer/scene.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

namespace Engine {

Player::Player(Camera &camera, InputManager &input)
    : m_camera(camera), m_input(input) {
  transform.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
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
    m_velocity.x = moveDir.x * m_moveSpeed;
    m_velocity.z = moveDir.z * m_moveSpeed;

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

  transform.setPosition(pos);
  m_camera.setPosition(pos + glm::vec3(0.0f, m_eyeHeight, 0.0f));
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
