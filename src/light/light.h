#pragma once
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <string>

namespace Engine {

class Light {
public:
  enum Type { POINT, DIRECTIONAL, SPOT };

  Type type;
  std::string name;
  bool enabled = true;
  glm::vec3 ambient{0.1f};
  glm::vec3 diffuse{0.8f};
  glm::vec3 specular{1.0f};

  Light(Type t, const std::string &n) : type(t), name(n) {}
  virtual ~Light() = default;

  virtual void renderUI() {
    ImGui::ColorEdit3("Ambient", &ambient.x, ImGuiColorEditFlags_NoInputs);
    ImGui::ColorEdit3("Diffuse", &diffuse.x, ImGuiColorEditFlags_NoInputs);
    ImGui::ColorEdit3("Specular", &specular.x, ImGuiColorEditFlags_NoInputs);
  }
};

class DirectionalLight : public Light {
public:
  DirectionalLight() : Light(DIRECTIONAL, "Directional") {}
  glm::vec3 direction{-0.2f, -1.0f, -0.3f};

  void renderUI() override {
    Light::renderUI();
    ImGui::DragFloat3("Direction", &direction.x, 0.01f);
  }
};

class PointLight : public Light {
public:
  PointLight() : Light(POINT, "Point") {}
  glm::vec3 position{1.0f, 3.0f, 1.0f};
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;

  void renderUI() override {
    Light::renderUI();
    ImGui::DragFloat3("Position", &position.x, 0.1f);
    ImGui::SliderFloat("Constant", &constant, 0.0f, 2.0f);
    ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f, "%.4f");
    ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f, "%.4f");
  }
};

class SpotLight : public Light {
public:
  SpotLight() : Light(SPOT, "Spot") {}
  glm::vec3 position{0.0f, 5.0f, 0.0f};
  glm::vec3 direction{0.0f, -1.0f, 0.0f};
  float cutOff = glm::cos(glm::radians(12.5f));
  float outerCutOff = glm::cos(glm::radians(17.5f));
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;

  void renderUI() override {
    Light::renderUI();
    ImGui::DragFloat3("Position", &position.x, 0.1f);
    ImGui::DragFloat3("Direction", &direction.x, 0.01f);
    float cutDeg = glm::degrees(glm::acos(cutOff));
    float outerDeg = glm::degrees(glm::acos(outerCutOff));
    if (ImGui::SliderFloat("Inner Angle", &cutDeg, 1.0f, 45.0f, "%.1f deg"))
      cutOff = glm::cos(glm::radians(cutDeg));
    if (ImGui::SliderFloat("Outer Angle", &outerDeg, 1.0f, 60.0f, "%.1f deg"))
      outerCutOff = glm::cos(glm::radians(outerDeg));
    ImGui::SliderFloat("Constant", &constant, 0.0f, 2.0f);
    ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f, "%.4f");
    ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f, "%.4f");
  }
};

} // namespace Engine
