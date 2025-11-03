#include "BlackHole.hpp"

namespace Sim {
void BlackHole::Draw(const Camera &camera) {
  m_ShaderProgram.Use();
  m_ShaderProgram.Set(camera.GetProjectionTransform(), "u_Proj");
  m_ShaderProgram.Set(camera.GetViewTransform(), "u_View");
  m_ShaderProgram.Set(m_Sphere.GetModel(), "u_Model");
  m_Sphere.Draw();
}
} // namespace Sim
