#include "BlackHole.hpp"
#include <chrono>
#include <cmath>

namespace Sim {

BlackHole::BlackHole()
    : m_StartTime(std::chrono::high_resolution_clock::now()) {}

void BlackHole::SetMass(float mass) {
  m_SolarMass = mass;
  m_EventHorizon = 2.0f * m_SolarMass;
}

void BlackHole::Draw(const Camera &camera, GLuint output_target) {
  glBindTexture(GL_TEXTURE_2D, output_target);
  GLint w = 0, h = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

  const glm::mat4 invProj = glm::inverse(camera.GetProjectionTransform());
  const glm::mat4 invView = glm::inverse(camera.GetViewTransform());

  m_ShaderProgram.Use();

  glBindImageTexture(0, output_target, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     GL_RGBA32F);
  m_Skybox.Bind(0);

  m_ShaderProgram.Set(invProj, "uInvProj");
  m_ShaderProgram.Set(invView, "uInvView");
  m_ShaderProgram.Set(camera.GetPosition(), "uCamPos");

  m_ShaderProgram.Set(glm::vec2(static_cast<float>(w), static_cast<float>(h)),
                      "uResolution");

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

  const GLuint WGX = 16, WGY = 16;
  GLuint gx = (w + WGX - 1) / WGX;
  GLuint gy = (h + WGY - 1) / WGY;

  glDispatchCompute(gx, gy, 1);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                  GL_TEXTURE_FETCH_BARRIER_BIT);
}

} // namespace Sim
