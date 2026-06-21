#include "transform.h"

namespace Engine {

Transform::Transform(const glm::vec3 &pos, const glm::vec3 &rot,
                     const glm::vec3 &scl)
    : m_position(pos), m_rotation(rot), m_scale(scl) {}

void Transform::setPosition(const glm::vec3 &pos) { m_position = pos; }

void Transform::setRotation(const glm::vec3 &rot) { m_rotation = rot; }

void Transform::setScale(const glm::vec3 &scl) { m_scale = scl; }

void Transform::translate(const glm::vec3 &offset) { m_position += offset; }

void Transform::rotate(const glm::vec3 &angles) { m_rotation += angles; }

const glm::mat4 &Transform::getModelMatrix() {
  m_modelMatrix = glm::mat4(1.0f);
  m_modelMatrix = glm::translate(m_modelMatrix, m_position);
  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x),
                              glm::vec3(1.0f, 0.0f, 0.0f));
  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y),
                              glm::vec3(0.0f, 1.0f, 0.0f));
  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z),
                              glm::vec3(0.0f, 0.0f, 1.0f));
  m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
  return m_modelMatrix;
}

} // namespace Engine
