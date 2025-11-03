#include "ShaderProgram.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace Sim {

static const std::unordered_map<std::string, GLuint> s_ShaderTypes = {
    {".vert", GL_VERTEX_SHADER},       {".frag", GL_FRAGMENT_SHADER},
    {".geom", GL_GEOMETRY_SHADER},     {".comp", GL_COMPUTE_SHADER},
    {".tesc", GL_TESS_CONTROL_SHADER}, {".tese", GL_TESS_EVALUATION_SHADER}};

ShaderProgram::ShaderProgram(
    std::initializer_list<std::filesystem::path> shader_files) {

  std::vector<std::string> shader_sources;
  std::vector<GLuint> shader_ids;

  shader_sources.reserve(shader_files.size());
  shader_ids.reserve(shader_files.size());

  for (auto &shader_file : shader_files) {
    shader_sources.push_back(LoadFromFile(shader_file));
  }

  for (size_t i = 0; i < shader_sources.size(); i++) {
    GLuint shader_type = GetShaderType((shader_files.begin() + i)->extension());
    shader_ids.push_back(CompileShader(shader_sources[i].c_str(), shader_type));
  }

  m_ShaderProgram = glCreateProgram();
  for (auto shader_id : shader_ids) {
    glAttachShader(m_ShaderProgram, shader_id);
  }

  glLinkProgram(m_ShaderProgram);

  char infoLog[512];
  GLint success;
  glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, infoLog);

    std::stringstream ss;
    ss << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
    throw std::runtime_error(ss.str());
  }

  for (auto shader_id : shader_ids) {
    glDeleteShader(shader_id);
  }
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_ShaderProgram); }

void ShaderProgram::Use() { glUseProgram(m_ShaderProgram); }

void ShaderProgram::Set(const glm::mat4 &matrix, const char *variable) {
  Use();
  GLint loc = glGetUniformLocation(m_ShaderProgram, variable);
  if (loc == -1) {
    return;
  }

  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string ShaderProgram::LoadFromFile(const std::filesystem::path &path) {
  std::ifstream shader_file(path);
  std::string content((std::istreambuf_iterator<char>(shader_file)),
                      (std::istreambuf_iterator<char>()));

  shader_file.close();

  return content;
}

GLuint ShaderProgram::CompileShader(const char *source_code, GLuint type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source_code, NULL);
  glCompileShader(shader);
  GLint success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    assert(false);
  };

  return shader;
}

GLuint ShaderProgram::GetShaderType(const std::filesystem::path &extension) {
  auto it = s_ShaderTypes.find(extension.string());
  if (it != s_ShaderTypes.end())
    return it->second;

  throw std::runtime_error("Unknown shader type: " + extension.string());
}

} // namespace Sim
