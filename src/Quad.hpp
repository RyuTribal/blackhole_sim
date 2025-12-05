#pragma once

#include "MeshBase.hpp"
#if defined(IS_WEB)
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif

namespace Sim {

class Quad : public Mesh {
public:
  Quad(const glm::vec3 &position, const glm::vec2 &scale);
  ~Quad();

  void Draw() override;

private:
  void SetupMesh();

private:
  GLuint m_VertexArrayObject = 0;
  GLuint m_VertexBufferObject = 0;
  GLuint m_ElementBufferObject = 0;
};

} // namespace Sim
