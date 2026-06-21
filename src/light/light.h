#pragma once
#include <glm/glm.hpp>
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
};

class DirectionalLight : public Light {
public:
  DirectionalLight() : Light(DIRECTIONAL, "Directional") {}
  glm::vec3 direction{-0.2f, -1.0f, -0.3f};
};

class PointLight : public Light {
public:
  PointLight() : Light(POINT, "Point") {}
  glm::vec3 position{1.0f, 3.0f, 1.0f};
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;
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
};

} // namespace Engine
