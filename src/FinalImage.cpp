#include "FinalImage.hpp"

namespace Sim {
FinalImage::FinalImage(int width, int height)
{
  glGenTextures(1, &m_OutputTexture);
  glBindTexture(GL_TEXTURE_2D, m_OutputTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  OnResize(width, height);

  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_OutputTexture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenVertexArrays(1, &m_DummyVAO);
}

FinalImage::~FinalImage() {
  glDeleteVertexArrays(1, &m_DummyVAO);
  glDeleteTextures(1, &m_OutputTexture);
  glDeleteFramebuffers(1, &m_FBO);
}

void FinalImage::OnResize(int width, int height) {
  m_Width = width;
  m_Height = height;
  glBindTexture(GL_TEXTURE_2D, m_OutputTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
               GL_FLOAT, nullptr);
};

void FinalImage::Draw(const Camera &camera) {

  auto camera_viewport = camera.GetViewPort();

  if (camera_viewport.x != m_Width || camera_viewport.y != m_Height) {
    OnResize(camera_viewport.x, camera_viewport.y);
  }

  m_ShaderProgram.Use();
  glUniform1i(glGetUniformLocation(m_ShaderProgram.GetID(), "uFinal"), 0);
  glDisable(GL_DEPTH_TEST);
#if defined(IS_WEB)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_OutputTexture);
#else
  glBindTextureUnit(0, m_OutputTexture);
#endif

  glBindVertexArray(m_DummyVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}
} // namespace Sim
