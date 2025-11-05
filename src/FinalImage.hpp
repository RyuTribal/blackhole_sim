#pragma once
#include "Camera.hpp"
#include "Quad.hpp"
#include "ShaderProgram.hpp"

namespace Sim {
class FinalImage {
public:
  FinalImage(int width, int height);
  ~FinalImage();

  void Draw(const Camera &camera);

  GLuint GetOutputTexture() { return m_OutputTexture; }

private:
  void OnResize(int width, int height);

private:
  ShaderProgram m_ShaderProgram{"assets/shaders/present.vert",
                                "assets/shaders/present.frag"};

  int m_Width, m_Height;

  GLuint m_OutputTexture;
  GLuint m_DummyVAO = 0;
};
} // namespace Sim
