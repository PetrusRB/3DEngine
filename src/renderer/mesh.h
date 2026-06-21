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
    void uploadInstances(const std::vector<glm::mat4>& matrices);
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
