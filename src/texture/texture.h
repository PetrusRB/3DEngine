#pragma once
#include <glad/glad.h>
#include <string>

namespace Engine {

class Texture {
public:
  Texture(const std::string &path);
  ~Texture();

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

  void bind(unsigned int unit = 0) const;
  GLuint id() const;

private:
  GLuint m_id = 0;
  int m_width = 0;
  int m_height = 0;
  int m_channels = 0;
};

} // namespace Engine
