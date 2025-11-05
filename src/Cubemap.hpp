#pragma once
#include <filesystem>
#include <glad/gl.h>

namespace Sim {
class Cubemap {
public:
  Cubemap(const std::filesystem::path &cubemap_folder);
  virtual ~Cubemap();

  Cubemap(const Cubemap &) = delete;
  Cubemap &operator=(const Cubemap &) = delete;

  Cubemap(Cubemap &&other) noexcept {
    m_CubemapID = other.m_CubemapID;
    other.m_CubemapID = 0;
  }
  Cubemap &operator=(Cubemap &&other) noexcept {
    if (this != &other) {
      if (m_CubemapID)
        glDeleteTextures(1, &m_CubemapID);
      m_CubemapID = other.m_CubemapID;
      other.m_CubemapID = 0;
    }
    return *this;
  }

  void Bind(uint32_t slot);

private:
  GLuint m_CubemapID;
};
} // namespace Sim
