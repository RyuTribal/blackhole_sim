#include "Sphere.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Sim {

namespace {

struct EdgeKey {
  GLuint v1;
  GLuint v2;

  bool operator==(const EdgeKey &other) const {
    return v1 == other.v1 && v2 == other.v2;
  }
};

struct EdgeKeyHash {
  std::size_t operator()(const EdgeKey &key) const {
    return (static_cast<std::size_t>(key.v1) << 32) ^
           static_cast<std::size_t>(key.v2);
  }
};

} // namespace

Sphere::Sphere(const glm::vec3 &position, const glm::vec3 &scale, float radius,
               int subdivisions)
    : Mesh(glm::scale(glm::translate(glm::mat4(1.0f), position), scale)) {
  if (radius <= 0.0f) {
    radius = 1.0f;
  }
  if (subdivisions < 0) {
    subdivisions = 0;
  }

  SetupMesh(radius, subdivisions);
}

Sphere::~Sphere() {
  glDeleteBuffers(1, &m_VertexBufferObject);
  glDeleteBuffers(1, &m_ElementBufferObject);
  glDeleteVertexArrays(1, &m_VertexArrayObject);
}

void Sphere::SetupMesh(float radius, int subdivisions) {
  std::vector<glm::vec3> positions;
  std::vector<GLuint> indices;

  const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f;

  positions = {
      glm::normalize(glm::vec3{-1.0f, phi, 0.0f}),
      glm::normalize(glm::vec3{1.0f, phi, 0.0f}),
      glm::normalize(glm::vec3{-1.0f, -phi, 0.0f}),
      glm::normalize(glm::vec3{1.0f, -phi, 0.0f}),

      glm::normalize(glm::vec3{0.0f, -1.0f, phi}),
      glm::normalize(glm::vec3{0.0f, 1.0f, phi}),
      glm::normalize(glm::vec3{0.0f, -1.0f, -phi}),
      glm::normalize(glm::vec3{0.0f, 1.0f, -phi}),

      glm::normalize(glm::vec3{phi, 0.0f, -1.0f}),
      glm::normalize(glm::vec3{phi, 0.0f, 1.0f}),
      glm::normalize(glm::vec3{-phi, 0.0f, -1.0f}),
      glm::normalize(glm::vec3{-phi, 0.0f, 1.0f}),
  };

  indices = {0,  11, 5,  0,  5,  1,  0,  1,  7,  0,  7,  10, 0,  10, 11,
             1,  5,  9,  5,  11, 4,  11, 10, 2, 10, 7,  6,  7,  1,  8,
             3,  9,  4,  3,  4,  2,  3,  2,  6,  3,  6,  8,  3,  8,  9,
             4,  9,  5,  2,  4,  11, 6,  2,  10, 8,  6,  7,  9,  8,  1};

  auto getMidpoint = [&](GLuint a, GLuint b,
                         std::unordered_map<EdgeKey, GLuint, EdgeKeyHash>
                             &cache) -> GLuint {
    EdgeKey key{std::min(a, b), std::max(a, b)};
    auto it = cache.find(key);
    if (it != cache.end()) {
      return it->second;
    }

    glm::vec3 midpoint =
        glm::normalize((positions[a] + positions[b]) * 0.5f);
    positions.push_back(midpoint);
    GLuint index = static_cast<GLuint>(positions.size() - 1);
    cache.emplace(key, index);
    return index;
  };

  for (int i = 0; i < subdivisions; ++i) {
    std::vector<GLuint> refined;
    refined.reserve(indices.size() * 4);
    std::unordered_map<EdgeKey, GLuint, EdgeKeyHash> midpointCache;
    midpointCache.reserve(indices.size());

    for (size_t j = 0; j < indices.size(); j += 3) {
      GLuint v1 = indices[j];
      GLuint v2 = indices[j + 1];
      GLuint v3 = indices[j + 2];

      GLuint a = getMidpoint(v1, v2, midpointCache);
      GLuint b = getMidpoint(v2, v3, midpointCache);
      GLuint c = getMidpoint(v3, v1, midpointCache);

      refined.insert(refined.end(),
                     {v1, a, c, v2, b, a, v3, c, b, a, b, c});
    }

    indices.swap(refined);
  }

  std::vector<Vertex> vertices;
  vertices.reserve(positions.size());
  for (const auto &unitPos : positions) {
    glm::vec3 normal = glm::normalize(unitPos);
    glm::vec3 position = normal * radius;
    vertices.emplace_back(position, glm::vec2{0.0f, 0.0f}, normal);
  }

  std::vector<float> vertexData;
  vertexData.reserve(vertices.size() * 8);
  for (const auto &vertex : vertices) {
    auto floats = vertex.ToFloat();
    vertexData.insert(vertexData.end(), floats.begin(), floats.end());
  }

  m_IndexCount = static_cast<GLsizei>(indices.size());

  glGenVertexArrays(1, &m_VertexArrayObject);
  glGenBuffers(1, &m_VertexBufferObject);
  glGenBuffers(1, &m_ElementBufferObject);

  glBindVertexArray(m_VertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)),
               vertexData.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
               indices.data(), GL_STATIC_DRAW);

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

void Sphere::Draw() {
  glBindVertexArray(m_VertexArrayObject);
  glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

} // namespace Sim
