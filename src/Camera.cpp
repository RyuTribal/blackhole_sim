#include "Camera.hpp"
#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Sim {

Camera::Camera() { Update(); }

void Camera::OnResize(const glm::vec2 &view_port) {
  m_ViewPort = view_port;
  Update();
}

void Camera::Rotate(float x_delta, float y_delta) {
  float yaw = m_InverseCamera ? x_delta : -x_delta;
  float pitch = m_InverseCamera ? y_delta : -y_delta;

  yaw *= m_Sensitivity;
  pitch *= m_Sensitivity;
  glm::quat qYaw =
      glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

  glm::quat qPitch =
      glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));

  m_Orientation = qYaw * m_Orientation;

  m_Orientation = m_Orientation * qPitch;

  m_Orientation = glm::normalize(m_Orientation);

  glm::vec3 currentUp = GetUp();
  // if (currentUp.y < 0.0f) {
  //   m_Orientation = m_Orientation * glm::inverse(qPitch);
  // }

  Update();
}

void Camera::Move(const glm::vec3 &move_vec) {
  m_Position += move_vec;
  Update();
}

void Camera::Update() {
  glm::mat4 rotate = glm::mat4_cast(glm::conjugate(m_Orientation));
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), -m_Position);
  m_ViewTransform = rotate * translate;

  const float aspect = std::max(m_ViewPort.x / m_ViewPort.y, 0.01f);
  m_ProjectionTransform =
      glm::perspective(glm::radians(m_FOVYDeg), aspect, m_Near, m_Far);
}

const glm::vec3 Camera::GetForward() {
  return glm::rotate(m_Orientation, glm::vec3(0.0f, 0.0f, -1.0f));
}

const glm::vec3 Camera::GetRight() {
  return glm::rotate(m_Orientation, glm::vec3(1.0f, 0.0f, 0.0f));
}

const glm::vec3 Camera::GetUp() {
  return glm::rotate(m_Orientation, glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace Sim
