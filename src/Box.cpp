#include "Box.hpp"
#include "MeshBase.hpp"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Sim {

namespace {
static const std::array<Vertex, 8> cubeVertices = {
    Vertex{
        {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-0.57735f, -0.57735f, -0.57735f}},
    Vertex{
        {0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.57735f, -0.57735f, -0.57735f}},
    Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.57735f, 0.57735f, -0.57735f}},
    Vertex{
        {-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {-0.57735f, 0.57735f, -0.57735f}},
    Vertex{
        {-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {-0.57735f, -0.57735f, 0.57735f}},
    Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.57735f, -0.57735f, 0.57735f}},
    Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.57735f, 0.57735f, 0.57735f}},
    Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {-0.57735f, 0.57735f, 0.57735f}}};

static const std::array<unsigned int, 36> cubeIndices = {
    // Back face
    0, 1, 2, 2, 3, 0,
    // Front face
    4, 5, 6, 6, 7, 4,
    // Left face
    4, 0, 3, 3, 7, 4,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Bottom face
    4, 5, 1, 1, 0, 4,
    // Top face
    3, 2, 6, 6, 7, 3};
} // namespace

Box::Box(const glm::vec3 &position, const glm::vec3 &scale)
    : Mesh(glm::scale(glm::translate(glm::mat4(1.0f), position), scale)) {
  SetupMesh();
}

Box::~Box() {
  glDeleteBuffers(1, &m_VertexBufferObject);
  glDeleteBuffers(1, &m_ElementBufferObject);
  glDeleteVertexArrays(1, &m_VertexArrayObject);
}

void Box::SetupMesh() {
  glGenVertexArrays(1, &m_VertexArrayObject);
  glGenBuffers(1, &m_VertexBufferObject);
  glGenBuffers(1, &m_ElementBufferObject);

  glBindVertexArray(m_VertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
  std::vector<float> vertexData;
  vertexData.reserve(cubeVertices.size() * 8);
  for (const auto &vertex : cubeVertices) {
    auto floats = vertex.ToFloat();
    vertexData.insert(vertexData.end(), floats.begin(), floats.end());
  }
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
               vertexData.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices.data(),
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

void Box::Draw() {
  glBindVertexArray(m_VertexArrayObject);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

} // namespace Sim
