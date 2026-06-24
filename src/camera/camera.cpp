#include "camera.h"
#include <glm/trigonometric.hpp>

namespace Engine {

Camera::Camera(glm::vec3 position) : m_position(position) { updateVectors(); }

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect, float near,
                                      float far) const {
  return glm::perspective(glm::radians(m_fov), aspect, near, far);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
  float velocity = m_speed * deltaTime;
  switch (direction) {
  case CAM_FORWARD:
    m_position += m_front * velocity;
    break;
  case CAM_BACKWARD:
    m_position -= m_front * velocity;
    break;
  case CAM_LEFT:
    m_position -= m_right * velocity;
    break;
  case CAM_RIGHT:
    m_position += m_right * velocity;
    break;
  case CAM_UP:
    m_position += m_up * velocity;
    break;
  case CAM_DOWN:
    m_position -= m_up * velocity;
    break;
  }
}

void Camera::processMouse(float xoffset, float yoffset) {
  m_yaw += xoffset * m_sensitivity;
  m_pitch += yoffset * m_sensitivity;

  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;

  updateVectors();
}

void Camera::setPosition(const glm::vec3 &position) { m_position = position; }

glm::vec3 Camera::getPosition() const { return m_position; }
glm::vec3 Camera::getFront() const { return m_front; }
glm::vec3 Camera::getRight() const { return m_right; }
float Camera::getFov() const { return m_fov; }
void Camera::setFov(float fov) { m_fov = fov; }

void Camera::setRoll(float newRoll) { m_roll = newRoll; }
void Camera::updateVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  front.y = sin(glm::radians(m_pitch));
  front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front = glm::normalize(front);
  m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
  m_up = glm::normalize(glm::cross(m_right, m_front));
  if (m_roll != 0.0f) {
    glm::mat4 rollMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_roll), m_front);
    m_right = glm::vec3(rollMatrix * glm::vec4(m_right, 0.0f));
    m_up = glm::vec3(rollMatrix * glm::vec4(m_up, 0.0f));
  }
}

} // namespace Engine
