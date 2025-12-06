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
  void OnResize(int width, int height);

  GLuint GetOutputTexture() { return m_OutputTexture; }
  GLuint GetFBO() { return m_FBO; }

private:
#if defined(IS_WEB)
  ShaderProgram m_ShaderProgram{"assets/shaders/fullscreen_web.vert", "assets/shaders/present_web.frag"};
#else
  ShaderProgram m_ShaderProgram{"assets/shaders/fullscreen.vert", "assets/shaders/present.frag"};
#endif

  int m_Width, m_Height;

  GLuint m_FBO = 0;
  GLuint m_OutputTexture;
  GLuint m_DummyVAO = 0;
};
} // namespace Sim
