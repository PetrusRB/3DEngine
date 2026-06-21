#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

namespace Engine {
class Cubemap {
public:
  void load(std::vector<std::string> faces);
  void render();

private:
  unsigned int m_skyboxVAO = 0;
  unsigned int m_skyboxVBO = 0;
  unsigned int m_cubemapTexture = 0;
};
} // namespace Engine
