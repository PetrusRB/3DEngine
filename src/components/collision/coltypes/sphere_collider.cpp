#include "sphere_collider.h"
#include <imgui/imgui.h>

namespace Engine {

SphereCollider::SphereCollider(float radius) : m_radius(radius) {}

AABB SphereCollider::getLocalAABB() const {
  return {-glm::vec3(m_radius), glm::vec3(m_radius)};
}

void SphereCollider::renderUI() {
  Collision::renderUI();
  ImGui::Separator();
  ImGui::DragFloat("Radius", &m_radius, 0.01f, 0.001f, 100.0f);
}

} // namespace Engine
