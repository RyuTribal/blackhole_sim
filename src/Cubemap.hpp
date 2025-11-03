#pragma once
#include "Box.hpp"
#include "Camera.hpp"
#include "ShaderProgram.hpp"
#include <filesystem>
#include <glad/gl.h>

namespace Sim {
class Cubemap {
public:
  Cubemap(const std::filesystem::path &cubemap_folder);
  virtual ~Cubemap();

  void Draw(const Camera &camera);

private:
  GLuint m_CubemapID;
  ShaderProgram m_ShaderProgram{"assets/shaders/cubemap.vert",
                                "assets/shaders/cubemap.frag"};
  Box m_BoxMesh{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};
};
} // namespace Sim
