#include "Camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include <algorithm>

namespace Sim {
Camera::Camera() { OnResize(m_ViewPort); }

void Camera::OnResize(const glm::vec2 &view_port) {
  m_ViewPort = view_port;
  Update();
}

void Camera::Update() {
  m_ViewTransform =
      glm::lookAt(m_Position, m_Position + glm::vec3{0.f, 0.f, -1.f},
                  glm::vec3{0.f, 1.f, 0.f});

  const float aspect = std::max(m_ViewPort.x / m_ViewPort.y, 0.01f);

  m_ProjectionTransform =
      glm::perspective(glm::radians(m_FOVYDeg), aspect, m_Near, m_Far);
}
} // namespace Sim
