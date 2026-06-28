#pragma once
#include "../../texture/texture.h"
#include <imgui/imgui.h>

namespace Engine {

struct Sprite {
  ImTextureID texture = (ImTextureID)0;
  ImVec2 uvMin{0.0f, 0.0f};
  ImVec2 uvMax{1.0f, 1.0f};

  static Sprite fromTexture(const Texture &tex) {
    return {(ImTextureID)(intptr_t)tex.id()};
  }

  void draw(const ImVec2 &pos, const ImVec2 &size) const {
    ImGui::GetWindowDrawList()->AddImage(texture, pos,
                                         ImVec2(pos.x + size.x, pos.y + size.y),
                                         uvMin, uvMax);
  }

  static void drawFilled(const ImVec2 &min, const ImVec2 &max,
                         ImU32 color) {
    ImGui::GetWindowDrawList()->AddRectFilled(min, max, color);
  }

  static void drawRect(const ImVec2 &min, const ImVec2 &max, ImU32 color,
                       float thickness = 1.0f) {
    ImGui::GetWindowDrawList()->AddRect(min, max, color, 0.0f,
                                        ImDrawFlags_RoundCornersAll, thickness);
  }

  static void drawProgressBar(float percent, const ImVec2 &min,
                              const ImVec2 &max, ImU32 bgColor,
                              ImU32 fillColor) {
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(min, max, bgColor);
    ImVec2 fillMax(max.x, min.y + (max.y - min.y) * percent);
    dl->AddRectFilled(min, fillMax, fillColor);
  }
};

} // namespace Engine
