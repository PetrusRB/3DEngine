#include "shadow.h"
#include "../shader/shader.h"
#include <algorithm>

namespace Engine {

ShadowMap::~ShadowMap() { destroy(); }

bool ShadowMap::init(unsigned int resolution) {
  m_resolution = resolution;

  glGenFramebuffers(1, &m_fbo);

  glGenTextures(1, &m_depthMap);
  glBindTexture(GL_TEXTURE_2D, m_depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_resolution,
               m_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         m_depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void ShadowMap::destroy() {
  if (m_depthMap) {
    glDeleteTextures(1, &m_depthMap);
    m_depthMap = 0;
  }
  if (m_fbo) {
    glDeleteFramebuffers(1, &m_fbo);
    m_fbo = 0;
  }
}

void ShadowMap::updateLightSpace(const glm::vec3 &lightDir) {
  float halfSize = 30.0f;
  glm::vec3 lightPos = -lightDir * 20.0f;

  m_lightProjection =
      glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1f, 100.0f);
  m_lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  m_lightSpaceMatrix = m_lightProjection * m_lightView;
}

void ShadowMap::begin(ShaderProgram &depthShader, const glm::vec3 &lightDir) {
  updateLightSpace(lightDir);

  glViewport(0, 0, m_resolution, m_resolution);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glClear(GL_DEPTH_BUFFER_BIT);

  depthShader.use();
  depthShader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
}

void ShadowMap::end() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void ShadowMap::bind(unsigned int textureUnit) const {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, m_depthMap);
}

void ShadowMap::setResolution(unsigned int res) {
  if (res == m_resolution)
    return;
  destroy();
  init(res);
}

} // namespace Engine
