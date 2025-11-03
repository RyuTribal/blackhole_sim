#pragma once

#include "Camera.hpp"
#include "ShaderProgram.hpp"
namespace Sim {
class BlackHole {
public:
  BlackHole() = default;
  ~BlackHole() = default;
  void Draw(const Camera &camera);

private:
  glm::vec3 m_Position{0.f, 0.f, -10.f};
  float m_EventHorizon = 1.f;
};
} // namespace Sim
