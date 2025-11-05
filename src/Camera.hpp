#pragma once
#include <glm/glm.hpp>

namespace Sim {
class Camera {
public:
  Camera();
  virtual ~Camera() = default;

  void OnResize(const glm::vec2 &view_port);

  const glm::mat4 &GetViewTransform() const { return m_ViewTransform; }
  const glm::mat4 &GetProjectionTransform() const {
    return m_ProjectionTransform;
  }

  const glm::vec2 GetViewPort() const { return m_ViewPort; }
  const glm::vec3 GetPosition() const { return m_Position; }
  const float GetNear() const { return m_Near; }
  const float GetFar() const { return m_Far; }

private:
  void Update();

private:
  glm::vec2 m_ViewPort{640.f, 480.f};
  glm::vec3 m_Position{0.f};
  glm::mat4 m_ViewTransform{1.f};
  glm::mat4 m_ProjectionTransform{1.f};

  // Projection variables
  float m_FOVYDeg = 75.f;
  float m_Near = 0.001f;
  float m_Far = 2000.f;
};
} // namespace Sim
