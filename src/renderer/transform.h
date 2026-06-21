#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

class Transform {
public:
  Transform() = default;
  Transform(const glm::vec3 &pos, const glm::vec3 &rot = glm::vec3(0.0f),
            const glm::vec3 &scl = glm::vec3(1.0f));

  void setPosition(const glm::vec3 &pos);
  void setRotation(const glm::vec3 &rot);
  void setScale(const glm::vec3 &scl);

  void translate(const glm::vec3 &offset);
  void rotate(const glm::vec3 &angles);

  const glm::vec3 &getPosition() const { return m_position; }
  const glm::vec3 &getRotation() const { return m_rotation; }
  const glm::vec3 &getScale() const { return m_scale; }

  const glm::mat4 &getModelMatrix();

  glm::vec3 m_position{0.0f};
  glm::vec3 m_rotation{0.0f};
  glm::vec3 m_scale{1.0f};

private:
  glm::mat4 m_modelMatrix{1.0f};
};

} // namespace Engine
