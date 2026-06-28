#pragma once
#include "ui_component.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>

namespace Engine {

class Entity;

class HealthBar : public UIComponent {
public:
  HealthBar() = default;
  explicit HealthBar(Entity *entity);

  void render() override;
  void setEntity(Entity *entity) { m_entity = entity; }
  Entity *getEntity() const { return m_entity; }

  void setSize(float width, float height) {
    m_width = width;
    m_height = height;
  }

private:
  Entity *m_entity = nullptr;
  float m_width = 200.0f;
  float m_height = 20.0f;
  float m_padding = 20.0f;

  static glm::vec3 healthColor(float percent);
  static ImU32 vec3ToImU32(const glm::vec3 &c, float a = 1.0f);
};

} // namespace Engine
