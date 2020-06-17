#ifndef PROGRAM_H
#define PROGRAM_H

#include <glad/glad.h>
#include <filesystem>
#include <glm/detail/type_mat4x4.hpp>
#include <unordered_map>
namespace engine {
class Program {
 public:
  Program(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
  ~Program();
  bool bind();
  void unbind();

  void set_float(const std::string& name, float value);
  void set_bool(const std::string& name, bool value);
  void set_int(const std::string& name, int value);
  void set_matrix(const std::string& names, const glm::mat4& mat);

  void set_vector(const std::string& name, const glm::vec2& vec);
  void set_vector(const std::string& name, const glm::vec3& vec);
  void set_vector(const std::string& name, const glm::vec4& vec);
  bool load();

  bool is_loaded();

 private:
  GLuint programID{0};
  std::vector<char> vertexShaderSource;
  std::vector<char> fragmentShaderSource;
  std::unordered_map<std::string, int> locations;
  bool loaded{false};
};
}  // namespace engine

#endif  // PROGRAM_H
