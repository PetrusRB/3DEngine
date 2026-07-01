#include "texture.h"
#include "stb_image.h"
#include <cstdint>
#include <cstdio>
#include <iostream>

namespace Engine {

static bool checkImageSize(const std::string &path, int maxSize) {
  FILE *f = fopen(path.c_str(), "rb");
  if (!f)
    return true;

  unsigned char header[24];
  bool ok = (fread(header, 1, 24, f) == 24);
  fclose(f);
  if (!ok)
    return true;

  if (header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4E &&
      header[3] == 0x47) {
    uint32_t w = (header[16] << 24) | (header[17] << 16) | (header[18] << 8) |
                 header[19];
    uint32_t h = (header[20] << 24) | (header[21] << 16) | (header[22] << 8) |
                 header[23];
    if (static_cast<int>(w) > maxSize || static_cast<int>(h) > maxSize) {
      std::cout << "Texture muito grande (" << w << "x" << h
                << "), max: " << maxSize << " - " << path << std::endl;
      return false;
    }
  }

  return true;
}

Texture::Texture(const std::string &path) {
  static constexpr int MAX_TEX_SIZE = 8192;

  if (!checkImageSize(path, MAX_TEX_SIZE))
    return;

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
  if (!data) {
    std::cout << "Texture falhou ao carregar: " << path << std::endl;
    return;
  }

  if (m_channels != 1 && m_channels != 3 && m_channels != 4) {
    std::cout << "Texture canais invalidos (" << m_channels << "): " << path
              << std::endl;
    stbi_image_free(data);
    return;
  }

  if (m_width <= 0 || m_height <= 0) {
    std::cout << "Texture dimensoes invalidas: " << path << std::endl;
    stbi_image_free(data);
    return;
  }

  GLenum format = (m_channels == 4) ? GL_RGBA : GL_RGB;

  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format,
               GL_UNSIGNED_BYTE, data);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::cout << "Texture OpenGL erro (0x" << std::hex << err << std::dec
              << "): " << path << std::endl;
    glDeleteTextures(1, &m_id);
    m_id = 0;
    stbi_image_free(data);
    return;
  }

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

Texture::~Texture() {
  if (m_id)
    glDeleteTextures(1, &m_id);
}

void Texture::bind(unsigned int unit) const {
  if (!m_id)
    return;
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_id);
}

GLuint Texture::id() const { return m_id; }

} // namespace Engine
