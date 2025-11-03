#include "Camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include <algorithm>

namespace Sim {
Camera::Camera() { Update(m_ViewBox); }

void Camera::Update(const glm::vec2 &view_box) {
  m_ViewBox = view_box;
  if (m_ViewBox.x <= 0.0f) {
    m_ViewBox.x = 1.0f;
  }
  if (m_ViewBox.y <= 0.0f) {
    m_ViewBox.y = 1.0f;
  }

  m_ViewTransform =
      glm::lookAt(m_Position, m_Position + glm::vec3{0.f, 0.f, -1.f},
                  glm::vec3{0.f, 1.f, 0.f});

  const float aspect = std::max(m_ViewBox.x / m_ViewBox.y, 0.01f);

  m_ProjectionTransform =
      glm::perspective(glm::radians(m_FOVYDeg), aspect, m_Near, m_Far);
}
} // namespace Sim
