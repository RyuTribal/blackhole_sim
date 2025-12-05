#include "BlackHole.hpp"
#include "FinalImage.hpp"
#include <chrono>
#include <cmath>

namespace Sim {

BlackHole::BlackHole()
    : m_StartTime(std::chrono::high_resolution_clock::now())
{
#if defined(IS_WEB)
    m_Quad = std::make_unique<Quad>(glm::vec3(0.0f), glm::vec2(2.0f));
#endif
}

void BlackHole::SetMass(float mass) {
  m_SolarMass = mass;
  m_EventHorizon = 2.0f * m_SolarMass;
}

void BlackHole::Draw(const Camera &camera, FinalImage &final_image) {
  const glm::mat4 invProj = glm::inverse(camera.GetProjectionTransform());
  const glm::mat4 invView = glm::inverse(camera.GetViewTransform());

  m_ShaderProgram.Use();

  m_Skybox.Bind(0);
  glUniform1i(glGetUniformLocation(m_ShaderProgram.GetID(), "uSkybox"), 0);

  m_ShaderProgram.Set(invProj, "uInvProj");
  m_ShaderProgram.Set(invView, "uInvView");
  m_ShaderProgram.Set(camera.GetPosition(), "uCamPos");

  auto viewport = camera.GetViewPort();
  m_ShaderProgram.Set(glm::vec2(viewport.x, viewport.y), "uResolution");

  m_ShaderProgram.Set(m_Position, "uBHPos");
  m_ShaderProgram.Set(m_EventHorizon, "uHorizonRadius");
  m_ShaderProgram.Set(m_SolarMass, "uBHMass");
  m_ShaderProgram.Set(m_MaxSteps, "uMaxSteps");

  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> elapsed = now - m_StartTime;
  m_ShaderProgram.Set(elapsed.count(), "uTime");

  m_ShaderProgram.Set(m_ShowDisk, "uShowDisk");
  m_ShaderProgram.Set(m_DiskHeight, "uDiskHeight");
  m_ShaderProgram.Set(m_DiskIntensity, "uDiskIntensity");
  m_ShaderProgram.Set(m_DiskAlpha, "uDiskAlpha");
  m_ShaderProgram.Set(m_BendFactor, "uBendFactor");

#if defined(IS_WEB)
  glBindFramebuffer(GL_FRAMEBUFFER, final_image.GetFBO());
  glViewport(0, 0, viewport.x, viewport.y);
  m_Quad->Draw();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
  glBindImageTexture(0, final_image.GetOutputTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     GL_RGBA32F);
  const GLuint WGX = 16, WGY = 16;
  GLuint gx = (viewport.x + WGX - 1) / WGX;
  GLuint gy = (viewport.y + WGY - 1) / WGY;

  glDispatchCompute(gx, gy, 1);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                  GL_TEXTURE_FETCH_BARRIER_BIT);
#endif
}

} // namespace Sim
