#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

class ShaderProgram;

class ShadowMap {
public:
  ShadowMap() = default;
  ~ShadowMap();

  ShadowMap(const ShadowMap &) = delete;
  ShadowMap &operator=(const ShadowMap &) = delete;

  bool init(unsigned int resolution = 2048);
  void destroy();

  void begin(ShaderProgram &depthShader, const glm::vec3 &lightDir);
  void end();

  void bind(unsigned int textureUnit = 1) const;

  const glm::mat4 &getLightSpaceMatrix() const { return m_lightSpaceMatrix; }
  unsigned int getDepthTexture() const { return m_depthMap; }
  unsigned int getResolution() const { return m_resolution; }

  void setResolution(unsigned int res);

private:
  void updateLightSpace(const glm::vec3 &lightDir);

  unsigned int m_fbo = 0;
  unsigned int m_depthMap = 0;
  unsigned int m_resolution = 2048;
  glm::mat4 m_lightSpaceMatrix{1.0f};
  glm::mat4 m_lightProjection{1.0f};
  glm::mat4 m_lightView{1.0f};
};

} // namespace Engine
