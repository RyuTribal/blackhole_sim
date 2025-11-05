#pragma once

#include "Camera.hpp"
#include "Cubemap.hpp"
#include "ShaderProgram.hpp"
namespace Sim {
class BlackHole {
public:
  static constexpr float LIGHT_SPEED = 299792458.0;
  static constexpr float GRAVITATIONAL_CONSTANT = 6.67430e-11;
  BlackHole();
  ~BlackHole() = default;
  void Draw(const Camera &camera, GLuint output_target);

private:
  Cubemap m_Skybox{"assets/cubemap"};
  glm::vec3 m_Position{0.f, 0.f, -10.f};
  float m_EventHorizon;
  float m_Mass = 8.54e36;
  float m_StepMin = 0.002f;
  float m_StepMax = 0.02f;
  int m_MaxSteps = 800;
  ShaderProgram m_ShaderProgram{"assets/shaders/blackhole.comp"};
};
} // namespace Sim
