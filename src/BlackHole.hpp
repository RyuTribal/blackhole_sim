#pragma once

#include "Camera.hpp"
#include "Cubemap.hpp"
#include "ShaderProgram.hpp"
#include <chrono>

namespace Sim {

class BlackHole {
public:
  // Geometric units to avoid floating point precision issues of small float
  // numbers
  static constexpr float LIGHT_SPEED = 1;
  static constexpr float GRAVITATIONAL_CONSTANT = 1;

  BlackHole();
  ~BlackHole() = default;

  void Draw(const Camera &camera, GLuint output_target);

  float GetMass() const { return m_SolarMass; }
  float GetEventHorizon() const { return m_EventHorizon; }
  void SetMass(float mass);

  // --- Physics & Position ---
  glm::vec3 m_Position{0.f, -5.f, -40.f};
  float m_StepMin = 0.02f;
  float m_StepMax = 2.0f;
  int m_MaxSteps = 650;

  // --- Accretion Disc Settings  ---
  bool m_ShowDisk = true;
  float m_DiskHeight = 0.35f;
  float m_DiskIntensity = 30.0f;
  float m_DiskAlpha = 3.0f;
  float m_BendFactor = 5.0f;

private:
  Cubemap m_Skybox{"assets/cubemap"};
  ShaderProgram m_ShaderProgram{"assets/shaders/blackhole.comp"};

  float m_SolarMass = 0.5f;
  float m_EventHorizon = 2.0f * m_SolarMass; // Rs = 2GM/c^2

  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
};

} // namespace Sim
