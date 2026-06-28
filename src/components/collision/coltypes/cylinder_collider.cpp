#include "cylinder_collider.h"
#include <imgui/imgui.h>

namespace Engine {

CylinderCollider::CylinderCollider(float radius, float height, CylinderAxis axis)
    : m_radius(radius), m_height(height), m_axis(axis) {}

AABB CylinderCollider::getLocalAABB() const {
  float halfH = m_height * 0.5f;
  switch (m_axis) {
  case CylinderAxis::X:
    return {{-halfH, -m_radius, -m_radius},
            {halfH, m_radius, m_radius}};
  case CylinderAxis::Y:
    return {{-m_radius, -halfH, -m_radius},
            {m_radius, halfH, m_radius}};
  case CylinderAxis::Z:
    return {{-m_radius, -m_radius, -halfH},
            {m_radius, m_radius, halfH}};
  }
  return {{-m_radius, -halfH, -m_radius}, {m_radius, halfH, m_radius}};
}

void CylinderCollider::renderUI() {
  Collision::renderUI();
  ImGui::Separator();
  ImGui::DragFloat("Radius", &m_radius, 0.01f, 0.001f, 100.0f);
  ImGui::DragFloat("Height", &m_height, 0.01f, 0.001f, 100.0f);

  const char *axisNames[] = {"X", "Y", "Z"};
  int currentAxis = static_cast<int>(m_axis);
  if (ImGui::Combo("Axis", &currentAxis, axisNames, 3)) {
    m_axis = static_cast<CylinderAxis>(currentAxis);
  }
}

} // namespace Engine
