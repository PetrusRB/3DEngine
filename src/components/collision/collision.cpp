#include "./collision.h"
#include <imgui/imgui.h>

namespace Engine {

void Collision::setPosition(const glm::vec3 &pos) {
  m_position = pos;
  m_offsetDirty = true;
}
void Collision::setRotation(const glm::vec3 &rot) {
  m_rotation = rot;
  m_offsetDirty = true;
}
void Collision::setScale(const glm::vec3 &scl) {
  m_scale = scl;
  m_offsetDirty = true;
}

const glm::mat4 &Collision::getOffsetMatrix() const {
  if (!m_offsetDirty)
    return m_offsetMatrix;
  m_offsetMatrix = glm::mat4(1.0f);
  m_offsetMatrix = glm::translate(m_offsetMatrix, m_position);
  m_offsetMatrix = glm::rotate(m_offsetMatrix, glm::radians(m_rotation.x),
                               glm::vec3(1, 0, 0));
  m_offsetMatrix = glm::rotate(m_offsetMatrix, glm::radians(m_rotation.y),
                               glm::vec3(0, 1, 0));
  m_offsetMatrix = glm::rotate(m_offsetMatrix, glm::radians(m_rotation.z),
                               glm::vec3(0, 0, 1));
  m_offsetMatrix = glm::scale(m_offsetMatrix, m_scale);
  m_offsetDirty = false;
  return m_offsetMatrix;
}

AABB Collision::getWorldAABB(const Transform &entityTransform) const {
  AABB local = getLocalAABB();
  glm::mat4 world = entityTransform.getModelMatrix() * getOffsetMatrix();

  glm::vec3 corners[8] = {
      {local.min.x, local.min.y, local.min.z},
      {local.max.x, local.min.y, local.min.z},
      {local.min.x, local.max.y, local.min.z},
      {local.max.x, local.max.y, local.min.z},
      {local.min.x, local.min.y, local.max.z},
      {local.max.x, local.min.y, local.max.z},
      {local.min.x, local.max.y, local.max.z},
      {local.max.x, local.max.y, local.max.z}};

  glm::vec3 wMin = glm::vec3(world * glm::vec4(corners[0], 1.0f));
  glm::vec3 wMax = wMin;

  for (int i = 1; i < 8; i++) {
    glm::vec3 wc = glm::vec3(world * glm::vec4(corners[i], 1.0f));
    wMin = glm::min(wMin, wc);
    wMax = glm::max(wMax, wc);
  }

  return {wMin, wMax};
}

void Collision::renderUI() {
  ImGui::DragFloat3("Offset Position", &m_position.x, 0.1f);
  ImGui::DragFloat3("Offset Rotation", &m_rotation.x, 1.0f);
  ImGui::DragFloat3("Offset Scale", &m_scale.x, 0.1f, 0.01f, 100.0f);
  ImGui::Checkbox("Trigger", &m_isTrigger);
  ImGui::SameLine();
  ImGui::Checkbox("Active", &m_isActive);
}

} // namespace Engine
