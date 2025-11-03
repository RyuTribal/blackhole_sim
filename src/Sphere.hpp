#pragma once

#include "MeshBase.hpp"
#include <glad/gl.h>

namespace Sim {

class Sphere : public Mesh {
public:
  Sphere(const glm::vec3 &position, const glm::vec3 &scale, float radius,
         int subdivisions);
  ~Sphere();

  void Draw() override;

private:
  void SetupMesh(float radius, int subdivisions);

private:
  GLuint m_VertexArrayObject = 0;
  GLuint m_VertexBufferObject = 0;
  GLuint m_ElementBufferObject = 0;
  GLsizei m_IndexCount = 0;
};

} // namespace Sim
