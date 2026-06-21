#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace Engine {

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;
    GLuint id() const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

private:
    GLuint m_id = 0;

    static std::string readFile(const std::string& path);
    static GLuint compileShader(const std::string& source, GLenum type);
};

} // namespace Engine
