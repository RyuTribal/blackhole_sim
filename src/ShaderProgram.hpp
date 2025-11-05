#pragma once
#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <initializer_list>

namespace Sim {
class ShaderProgram {
public:
  ShaderProgram(std::initializer_list<std::filesystem::path> shader_files);
  virtual ~ShaderProgram();

  void Use();

  void Set(const glm::mat4 &matrix, const char *variable);
  void Set(const glm::vec3 &vector, const char *variable);
  void Set(const glm::vec2 &vector, const char *variable);
  void Set(const float &number, const char *variable);
  void Set(const int &number, const char *variable);

private:
  std::string LoadFromFile(const std::filesystem::path &path);
  GLuint CompileShader(const char *source_code, GLuint type);
  GLuint GetShaderType(const std::filesystem::path &extension);

private:
  GLuint m_ShaderProgram;
};
} // namespace Sim
