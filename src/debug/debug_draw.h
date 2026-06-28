#pragma once
#include "../entity/entity.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Engine {

class ShaderProgram;
class Collision;
class Transform;

class DebugDraw {
public:
  DebugDraw() = default;
  ~DebugDraw();

  DebugDraw(const DebugDraw &) = delete;
  DebugDraw &operator=(const DebugDraw &) = delete;

  void init();
  void destroy();

  void begin();

  void drawBox(const glm::mat4 &world, const glm::vec3 &halfExtents,
               const glm::vec3 &color);
  void drawAABB(const AABB &aabb, const glm::vec3 &color);
  void drawSphere(const glm::vec3 &center, float radius,
                  const glm::vec3 &color, int segments = 16);
  void drawCylinder(const glm::mat4 &world, float radius, float height,
                    const glm::vec3 &color, int segments = 16);
  void drawCircle(const glm::vec3 &center, float radius,
                  const glm::vec3 &normal, const glm::vec3 &color,
                  int segments = 16);

  void drawCollision(const Collision &col, const Transform &entityTransform,
                     const glm::vec3 &color);

  void end(const glm::mat4 &view, const glm::mat4 &projection);

  size_t lineCount() const { return m_lines.size() / 2; }

private:
  struct LineVertex {
    glm::vec3 pos;
    glm::vec3 color;
  };

  GLuint m_vao = 0;
  GLuint m_vbo = 0;
  std::vector<LineVertex> m_lines;
  std::unique_ptr<ShaderProgram> m_shader;

  void pushLine(const glm::vec3 &a, const glm::vec3 &b,
                const glm::vec3 &color);
  glm::vec3 transformPoint(const glm::mat4 &m, const glm::vec3 &p);
};

} // namespace Engine
