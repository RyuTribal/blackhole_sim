#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Sim {
class Camera {
public:
  Camera();
  virtual ~Camera() = default;

  void OnResize(const glm::vec2 &view_port);
  void Rotate(const float yaw, const float pitch);
  void Move(const glm::vec3 &move_vec);

  const glm::mat4 &GetViewTransform() const { return m_ViewTransform; }
  const glm::mat4 &GetProjectionTransform() const {
    return m_ProjectionTransform;
  }

  const glm::vec2 GetViewPort() const { return m_ViewPort; }
  const glm::vec3 GetPosition() const { return m_Position; }
  const float GetNear() const { return m_Near; }
  const float GetFar() const { return m_Far; }
  const glm::vec3 GetForward();
  const glm::vec3 GetRight();
  const glm::vec3 GetUp();
  const float GetSensitivity() { return m_Sensitivity * 1000.f; }
  const bool GetInverseCamera() { return m_InverseCamera; }

  void SetSensitivity(float sensitivity) {
    m_Sensitivity = sensitivity / 1000.f;
  }
  void SetInverseCamera(bool inverse) { m_InverseCamera = inverse; }

private:
  void Update();
  glm::mat4 GetViewMatrix();

private:
  glm::vec2 m_ViewPort{640.f, 480.f};
  glm::vec3 m_Position{0.f};
  glm::mat4 m_ViewTransform{1.f};
  glm::mat4 m_ProjectionTransform{1.f};

  glm::quat m_Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  // Projection variables
  float m_FOVYDeg = 75.f;
  float m_Near = 0.001f;
  float m_Far = 2000.f;

  float m_Sensitivity = 0.1f;

  bool m_InverseCamera = false;
};
} // namespace Sim
