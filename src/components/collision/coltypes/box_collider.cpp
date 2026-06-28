#include "box_collider.h"
#include <imgui/imgui.h>

namespace Engine {

BoxCollider::BoxCollider(const glm::vec3 &halfExtents)
    : m_halfExtents(halfExtents) {}

AABB BoxCollider::getLocalAABB() const {
  return {-m_halfExtents, m_halfExtents};
}

void BoxCollider::renderUI() {
  Collision::renderUI();
  ImGui::Separator();
  ImGui::DragFloat3("Half Extents", &m_halfExtents.x, 0.01f, 0.001f, 100.0f);
}

} // namespace Engine
