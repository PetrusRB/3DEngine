#include "health_bar.h"
#include "../../entity/entity.h"
#include "sprite.h"
#include <imgui/imgui.h>

namespace Engine {

HealthBar::HealthBar(Entity *entity) : m_entity(entity) {}

void HealthBar::render() {
  if (!m_entity || !visible)
    return;

  float percent =
      glm::clamp(m_entity->health / m_entity->max_health, 0.0f, 1.0f);

  ImVec2 display = ImGui::GetIO().DisplaySize;
  ImVec2 barMin(m_padding, display.y - m_height - m_padding);
  ImVec2 barMax(barMin.x + m_width, barMin.y + m_height);

  glm::vec3 col = healthColor(percent);
  ImU32 fillColor = vec3ToImU32(col);
  ImU32 bgColor = IM_COL32(30, 30, 30, 200);
  ImU32 borderColor = IM_COL32(80, 80, 80, 255);

  Sprite::drawProgressBar(percent, barMin, barMax, bgColor, fillColor);
  Sprite::drawRect(barMin, barMax, borderColor, 2.0f);

  char label[32];
  snprintf(label, sizeof(label), "%.0f / %.0f", m_entity->health,
           m_entity->max_health);
  ImVec2 textSize = ImGui::CalcTextSize(label);
  ImVec2 textPos(barMin.x + (m_width - textSize.x) * 0.5f,
                 barMin.y + (m_height - textSize.y) * 0.5f);
  ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255),
                                      label);
}

glm::vec3 HealthBar::healthColor(float percent) {
  if (percent > 0.5f)
    return glm::mix(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                    (percent - 0.5f) * 2.0f);
  return glm::mix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
                  percent * 2.0f);
}

ImU32 HealthBar::vec3ToImU32(const glm::vec3 &c, float a) {
  return IM_COL32(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255),
                  static_cast<int>(c.b * 255), static_cast<int>(a * 255));
}

} // namespace Engine
