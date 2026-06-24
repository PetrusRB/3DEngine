#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace Engine {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    template <typename Allocator>
    void uploadInstances(const std::vector<glm::mat4, Allocator>& matrices) {
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
    void drawInstanced(int count) const;
    GLuint vao() const;

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLuint m_instanceVBO = 0;
    GLsizei m_vertexCount = 0;
    GLsizei m_indexCount = 0;
    bool m_indexed = false;
};

} // namespace Engine
