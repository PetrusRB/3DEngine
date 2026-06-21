#include "mesh.h"
#include <cstddef>

namespace Engine {

Mesh::Mesh(const std::vector<Vertex> &vertices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size())) {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoords));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices)
    : m_vertexCount(static_cast<GLsizei>(vertices.size())),
      m_indexCount(static_cast<GLsizei>(indices.size())), m_indexed(true) {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoords));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  if (m_instanceVBO)
    glDeleteBuffers(1, &m_instanceVBO);
  if (m_ebo)
    glDeleteBuffers(1, &m_ebo);
  if (m_vbo)
    glDeleteBuffers(1, &m_vbo);
  if (m_vao)
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::draw() const {
  glBindVertexArray(m_vao);
  if (m_indexed)
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
  else
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
  glBindVertexArray(0);
}

void Mesh::uploadInstances(const std::vector<glm::mat4> &matrices) {
  if (matrices.empty())
    return;

  if (!m_instanceVBO)
    glGenBuffers(1, &m_instanceVBO);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
               matrices.data(), GL_DYNAMIC_DRAW);

  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(4 + i);
    glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(i * sizeof(glm::vec4)));
    glVertexAttribDivisor(4 + i, 1);
  }

  glBindVertexArray(0);
}

void Mesh::drawInstanced(int count) const {
  glBindVertexArray(m_vao);
  if (m_indexed)
    glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0,
                            count);
  else
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, count);
  glBindVertexArray(0);
}

GLuint Mesh::vao() const { return m_vao; }

} // namespace Engine
