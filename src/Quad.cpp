#include "Quad.hpp"
#include "MeshBase.hpp"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Sim {

// clang-format off
static constexpr std::array<glm::vec3, 4> kQuadPositions{
    glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{0.5f, -0.5f, 0.0f},
    glm::vec3{0.5f, 0.5f, 0.0f},   glm::vec3{-0.5f, 0.5f, 0.0f}};

static constexpr std::array<glm::vec2, 4> kQuadUVs{
    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f},
    glm::vec2{0.0f, 1.0f}};

static constexpr unsigned int quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};
// clang-format on

Quad::Quad(const glm::vec3 &position, const glm::vec2 &scale)
    : Mesh(glm::scale(glm::translate(glm::mat4(1.0f), position),
                      glm::vec3{scale, 1.0f})) {
  SetupMesh();
}

Quad::~Quad() {
  glDeleteBuffers(1, &m_VertexBufferObject);
  glDeleteBuffers(1, &m_ElementBufferObject);
  glDeleteVertexArrays(1, &m_VertexArrayObject);
}

void Quad::SetupMesh() {
  glGenVertexArrays(1, &m_VertexArrayObject);
  glGenBuffers(1, &m_VertexBufferObject);
  glGenBuffers(1, &m_ElementBufferObject);

  glBindVertexArray(m_VertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
  std::vector<Vertex> vertices;
  vertices.reserve(kQuadPositions.size());

  for (size_t i = 0; i < kQuadPositions.size(); ++i) {
    vertices.emplace_back(kQuadPositions[i], kQuadUVs[i],
                          glm::vec3{0.0f, 0.0f, 1.0f});
  }

  std::vector<float> vertexData;
  vertexData.reserve(vertices.size() * 8);
  for (const auto &vertex : vertices) {
    auto floats = vertex.ToFloat();
    vertexData.insert(vertexData.end(), floats.begin(), floats.end());
  }

  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
               vertexData.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(5 * sizeof(float)));

  glBindVertexArray(0);
}

void Quad::Draw() {
  glBindVertexArray(m_VertexArrayObject);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

} // namespace Sim
