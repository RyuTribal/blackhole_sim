#pragma once

#include "glm/glm.hpp"
#include <vector>
namespace Sim {
struct Vertex {
  glm::vec3 Position{0.f, 0.f, 0.f};
  glm::vec2 UV{0.f, 0.f};
  glm::vec3 Normal{1.f, 1.f, 1.f};

  Vertex(const glm::vec3 &pos, const glm::vec2 &uv, const glm::vec3 &normal)
      : Position(pos), UV(uv), Normal(normal) {}

  std::vector<float> ToFloat() const {
    std::vector<float> vertex_vector;
    vertex_vector.reserve(8);

    vertex_vector.push_back(Position.x);
    vertex_vector.push_back(Position.y);
    vertex_vector.push_back(Position.z);
    vertex_vector.push_back(UV.x);
    vertex_vector.push_back(UV.y);
    vertex_vector.push_back(Normal.x);
    vertex_vector.push_back(Normal.y);
    vertex_vector.push_back(Normal.z);

    return vertex_vector;
  }
};

class Mesh {
public:
  virtual ~Mesh() = default;

  virtual void Draw() = 0;

  const glm::mat4 &GetModel() const { return m_Model; }
  const glm::mat4 &GetModelMatrix() const { return m_Model; }

protected:
  Mesh() = default;
  explicit Mesh(const glm::mat4 &model) : m_Model(model) {}

  glm::mat4 m_Model{1.0f};
};
} // namespace Sim
