#include "Cubemap.hpp"
#include <array>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Sim {
Cubemap::Cubemap(const std::filesystem::path &cubemap_folder) {
  stbi_set_flip_vertically_on_load(false);
  std::array<std::filesystem::path, 6> faces = {
      cubemap_folder / "px.png", // +X
      cubemap_folder / "nx.png", // -X
      cubemap_folder / "py.png", // +Y
      cubemap_folder / "ny.png", // -Y
      cubemap_folder / "pz.png", // +Z
      cubemap_folder / "nz.png"  // -Z
  };

  glGenTextures(1, &m_CubemapID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);

  int w, h, n;
  for (GLuint i = 0; i < faces.size(); ++i) {
    unsigned char *data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);
    if (!data) {
      glDeleteTextures(1, &m_CubemapID);
      assert(false);
    }

    GLenum format = (n == 1) ? GL_RED : (n == 3) ? GL_RGB : GL_RGBA;
    GLenum internalFormat = (format == GL_RGB) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0,
                 format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap() { glDeleteTextures(1, &m_CubemapID); }

void Cubemap::Draw(const Camera &camera) {
  glDepthFunc(GL_LEQUAL);
  m_ShaderProgram.Use();
  m_ShaderProgram.Set(camera.GetViewTransform(), "u_View");
  m_ShaderProgram.Set(camera.GetProjectionTransform(), "u_Proj");

  glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
  m_BoxMesh.Draw();
  glDepthFunc(GL_LESS);
}

} // namespace Sim
