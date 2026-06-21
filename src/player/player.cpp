#include "player.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine {

Player::Player(Camera &camera, InputManager &input)
    : m_camera(camera), m_input(input) {}

void Player::update(float deltaTime) {
  if (m_input.isKeyHeld(GLFW_KEY_W))
    m_camera.processKeyboard(CAM_FORWARD, deltaTime);
  if (m_input.isKeyHeld(GLFW_KEY_S))
    m_camera.processKeyboard(CAM_BACKWARD, deltaTime);
  if (m_input.isKeyHeld(GLFW_KEY_A))
    m_camera.processKeyboard(CAM_LEFT, deltaTime);
  if (m_input.isKeyHeld(GLFW_KEY_D))
    m_camera.processKeyboard(CAM_RIGHT, deltaTime);
  if (m_input.isKeyHeld(GLFW_KEY_SPACE))
    m_camera.processKeyboard(CAM_UP, deltaTime);
  if (m_input.isKeyHeld(GLFW_KEY_LEFT_SHIFT))
    m_camera.processKeyboard(CAM_DOWN, deltaTime);

  m_input.updateMouseDelta();
  glm::dvec2 delta = m_input.getMouseDelta();
  if (delta.x != 0.0 || delta.y != 0.0)
    m_camera.processMouse(static_cast<float>(delta.x),
                          static_cast<float>(delta.y));
}

glm::vec3 Player::getPosition() const { return m_camera.getPosition(); }

} // namespace Engine
