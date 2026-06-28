#include "debug_draw.h"
#include "../components/collision/collision.h"
#include "../components/collision/coltypes/box_collider.h"
#include "../components/collision/coltypes/cylinder_collider.h"
#include "../components/collision/coltypes/sphere_collider.h"
#include "../shader/shader.h"
#include "../utils/utils.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

DebugDraw::~DebugDraw() { destroy(); }

void DebugDraw::init() {
  m_shader =
      std::make_unique<ShaderProgram>("src/assets/shaders/debug.vs",
                                      "src/assets/shaders/debug.fs");

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                        (void *)offsetof(LineVertex, color));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void DebugDraw::destroy() {
  if (m_vbo)
    glDeleteBuffers(1, &m_vbo);
  if (m_vao)
    glDeleteVertexArrays(1, &m_vao);
  m_vbo = 0;
  m_vao = 0;
  m_shader.reset();
}

void DebugDraw::begin() { m_lines.clear(); }

void DebugDraw::pushLine(const glm::vec3 &a, const glm::vec3 &b,
                         const glm::vec3 &color) {
  m_lines.push_back({a, color});
  m_lines.push_back({b, color});
}

glm::vec3 DebugDraw::transformPoint(const glm::mat4 &m, const glm::vec3 &p) {
  return glm::vec3(m * glm::vec4(p, 1.0f));
}

void DebugDraw::drawBox(const glm::mat4 &world, const glm::vec3 &he,
                        const glm::vec3 &color) {
  glm::vec3 c[8] = {
      transformPoint(world, {-he.x, -he.y, -he.z}),
      transformPoint(world, {he.x, -he.y, -he.z}),
      transformPoint(world, {he.x, he.y, -he.z}),
      transformPoint(world, {-he.x, he.y, -he.z}),
      transformPoint(world, {-he.x, -he.y, he.z}),
      transformPoint(world, {he.x, -he.y, he.z}),
      transformPoint(world, {he.x, he.y, he.z}),
      transformPoint(world, {-he.x, he.y, he.z}),
  };

  // bottom
  pushLine(c[0], c[1], color);
  pushLine(c[1], c[2], color);
  pushLine(c[2], c[3], color);
  pushLine(c[3], c[0], color);
  // top
  pushLine(c[4], c[5], color);
  pushLine(c[5], c[6], color);
  pushLine(c[6], c[7], color);
  pushLine(c[7], c[4], color);
  // verticals
  pushLine(c[0], c[4], color);
  pushLine(c[1], c[5], color);
  pushLine(c[2], c[6], color);
  pushLine(c[3], c[7], color);
}

void DebugDraw::drawAABB(const AABB &aabb, const glm::vec3 &color) {
  glm::vec3 c[8] = {
      {aabb.min.x, aabb.min.y, aabb.min.z},
      {aabb.max.x, aabb.min.y, aabb.min.z},
      {aabb.max.x, aabb.max.y, aabb.min.z},
      {aabb.min.x, aabb.max.y, aabb.min.z},
      {aabb.min.x, aabb.min.y, aabb.max.z},
      {aabb.max.x, aabb.min.y, aabb.max.z},
      {aabb.max.x, aabb.max.y, aabb.max.z},
      {aabb.min.x, aabb.max.y, aabb.max.z},
  };

  pushLine(c[0], c[1], color);
  pushLine(c[1], c[2], color);
  pushLine(c[2], c[3], color);
  pushLine(c[3], c[0], color);
  pushLine(c[4], c[5], color);
  pushLine(c[5], c[6], color);
  pushLine(c[6], c[7], color);
  pushLine(c[7], c[4], color);
  pushLine(c[0], c[4], color);
  pushLine(c[1], c[5], color);
  pushLine(c[2], c[6], color);
  pushLine(c[3], c[7], color);
}

void DebugDraw::drawSphere(const glm::vec3 &center, float radius,
                           const glm::vec3 &color, int seg) {
  drawCircle(center, radius, glm::vec3(1, 0, 0), color, seg);
  drawCircle(center, radius, glm::vec3(0, 1, 0), color, seg);
  drawCircle(center, radius, glm::vec3(0, 0, 1), color, seg);
}

void DebugDraw::drawCircle(const glm::vec3 &center, float radius,
                           const glm::vec3 &normal, const glm::vec3 &color,
                           int seg) {
  glm::vec3 tangent;
  if (std::abs(normal.y) < 0.99f)
    tangent = glm::normalize(glm::cross(normal, glm::vec3(0, 1, 0)));
  else
    tangent = glm::normalize(glm::cross(normal, glm::vec3(1, 0, 0)));
  glm::vec3 bitangent = glm::cross(normal, tangent);

  for (int i = 0; i < seg; i++) {
    float a0 = (2.0f * M_PI * i) / seg;
    float a1 = (2.0f * M_PI * (i + 1)) / seg;
    glm::vec3 p0 = center + (tangent * std::cos(a0) + bitangent * std::sin(a0)) * radius;
    glm::vec3 p1 = center + (tangent * std::cos(a1) + bitangent * std::sin(a1)) * radius;
    pushLine(p0, p1, color);
  }
}

void DebugDraw::drawCylinder(const glm::mat4 &world, float radius, float height,
                             const glm::vec3 &color, int seg) {
  float halfH = height * 0.5f;

  auto circleInPlane = [&](float y) {
    glm::vec3 center = transformPoint(world, glm::vec3(0, y, 0));
    glm::vec3 nUp = glm::normalize(glm::vec3(world[1]));
    glm::vec3 nRight = glm::normalize(glm::vec3(world[0]));
    float s = glm::length(glm::vec3(world[0]));
    float r = radius * s;

    glm::vec3 tan = nRight;
    glm::vec3 bit = glm::cross(nUp, tan);

    for (int i = 0; i < seg; i++) {
      float a0 = (2.0f * M_PI * i) / seg;
      float a1 = (2.0f * M_PI * (i + 1)) / seg;
      glm::vec3 p0 = center + (tan * std::cos(a0) + bit * std::sin(a0)) * r;
      glm::vec3 p1 = center + (tan * std::cos(a1) + bit * std::sin(a1)) * r;
      pushLine(p0, p1, color);
    }
  };

  circleInPlane(-halfH);
  circleInPlane(halfH);

  glm::vec3 nRight = glm::normalize(glm::vec3(world[0]));
  float s = glm::length(glm::vec3(world[0]));
  float r = radius * s;
  glm::vec3 nUp = glm::normalize(glm::vec3(world[1]));
  glm::vec3 bit = glm::cross(nUp, nRight);

  for (int i = 0; i < 4; i++) {
    float a = (2.0f * M_PI * i) / 4.0f;
    glm::vec3 offset = (nRight * std::cos(a) + bit * std::sin(a)) * r;
    glm::vec3 bottom = transformPoint(world, glm::vec3(0, -halfH, 0)) + offset;
    glm::vec3 top = transformPoint(world, glm::vec3(0, halfH, 0)) + offset;
    pushLine(bottom, top, color);
  }
}

void DebugDraw::drawCollision(const Collision &col, const Transform &entityT,
                              const glm::vec3 &color) {
  glm::mat4 world = entityT.getModelMatrix() * col.getOffsetMatrix();

  switch (col.getType()) {
  case CollisionType::Box: {
    auto &box = static_cast<const BoxCollider &>(col);
    drawBox(world, box.getHalfExtents(), color);
    break;
  }
  case CollisionType::Sphere: {
    auto &sphere = static_cast<const SphereCollider &>(col);
    glm::vec3 center = glm::vec3(world[3]);
    float s = glm::length(glm::vec3(world[0]));
    drawSphere(center, sphere.getRadius() * s, color);
    break;
  }
  case CollisionType::Cylinder: {
    auto &cyl = static_cast<const CylinderCollider &>(col);
    drawCylinder(world, cyl.getRadius(), cyl.getHeight(), color);
    break;
  }
  }
}

void DebugDraw::end(const glm::mat4 &view, const glm::mat4 &projection) {
  if (m_lines.empty())
    return;

  m_shader->use();
  m_shader->setMat4("viewProj", projection * view);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(LineVertex),
               m_lines.data(), GL_DYNAMIC_DRAW);

  glLineWidth(1.5f);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_lines.size()));
  glLineWidth(1.0f);

  glBindVertexArray(0);
}

} // namespace Engine
