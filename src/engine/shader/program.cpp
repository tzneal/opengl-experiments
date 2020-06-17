#include "program.h"
#include <engine/assets.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include "glerror.h"
using namespace engine;
Program::Program(const std::filesystem::path& vertexPath,
                 const std::filesystem::path& fragmentPath) {
  vertexShaderSource = read_file(vertexPath);
  fragmentShaderSource = read_file(fragmentPath);
}

Program::~Program() {
  if (programID) {
    spdlog::info("delete program {}", programID);
    glDeleteProgram(programID);
  }
}
bool Program::bind() {
  glUseProgram(programID);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    spdlog::error("error binding shader {}: {}", programID, glErrorString(error));
    printProgramLog(programID);
    return false;
  }

  return true;
}
void Program::unbind() { glUseProgram(0); }
bool Program::load() {
  programID = glCreateProgram();
  spdlog::info("loading shader program {}", programID);
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  const GLchar* vertexShaderArray[] = {&vertexShaderSource[0]};
  const GLint vertexLengths[] = {static_cast<GLint>(vertexShaderSource.size())};
  // Set vertex source
  glShaderSource(vertexShader, 1, vertexShaderArray, vertexLengths);

  // Compile vertex source
  glCompileShader(vertexShader);

  // Check vertex shader for errors
  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if (vShaderCompiled != GL_TRUE) {
    spdlog::error("Unable to compile vertex shader {}", vertexShader);
    printShaderLog(vertexShader);
    return false;
  }

  // Attach vertex shader to program
  glAttachShader(programID, vertexShader);

  // Create fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  // Get fragment source
  const GLchar* fragmentShaderArray[] = {&fragmentShaderSource[0]};
  const GLint fragmentLengths[] = {static_cast<GLint>(fragmentShaderSource.size())};
  // Set fragment source
  glShaderSource(fragmentShader, 1, fragmentShaderArray, fragmentLengths);

  // Compile fragment source
  glCompileShader(fragmentShader);

  // Check fragment shader for errors
  GLint fShaderCompiled = GL_FALSE;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
  if (fShaderCompiled != GL_TRUE) {
    spdlog::error("Unable to compile fragment shader {}", fragmentShader);
    printShaderLog(fragmentShader);
    exit(0);
    return false;
  }

  // Attach fragment shader to program
  glAttachShader(programID, fragmentShader);

  // Link program
  glLinkProgram(programID);

  // Check for errors
  GLint programSuccess = GL_TRUE;
  glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE) {
    spdlog::error("Error linking program {}", programID);
    printProgramLog(programID);
    return false;
  }

  // shaders no longer needed after linking
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  loaded = true;
  return true;
}

template <typename T>
void set_value(std::unordered_map<std::string, int>& locations, int programID,
               const std::string& name, T value) {
  static_assert(std::is_same<T, int>() || std::is_same<T, glm::mat4>() ||
                    std::is_same<T, glm::vec4>() || std::is_same<T, glm::vec3>() ||
                    std::is_same<T, glm::vec2>() || std::is_same<T, float>(),
                "unsupported type");

  // do we know where it's at
  auto match = locations.find(name);
  int location = -1;
  if (match != locations.end()) {
    location = match->second;
  }

  // find it
  if (location == -1) {
    location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
      spdlog::error("{} is not a valid glsl program variable", name);
      return;
    }
  }

  // set the value
  locations[name] = location;
  if constexpr (std::is_same<T, int>()) {
    glUniform1i(location, value);
  } else if constexpr (std::is_same<T, float>()) {
    glUniform1f(location, value);
  } else if constexpr (std::is_same<T, glm::mat4>()) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  } else if constexpr (std::is_same<T, glm::vec4>()) {
    glUniform4f(location, value.r, value.g, value.b, value.a);
  } else if constexpr (std::is_same<T, glm::vec3>()) {
    glUniform3f(location, value.x, value.y, value.z);
  } else if constexpr (std::is_same<T, glm::vec2>()) {
    glUniform2f(location, value.x, value.y);
  }
}
void Program::set_bool(const std::string& name, bool value) {
  set_value<int>(locations, programID, name, static_cast<int>(value));
}
void Program::set_int(const std::string& name, int value) {
  set_value<int>(locations, programID, name, value);
}
void Program::set_float(const std::string& name, float value) {
  set_value<float>(locations, programID, name, value);
}
void Program::set_matrix(const std::string& name, const glm::mat4& mat) {
  set_value<glm::mat4>(locations, programID, name, mat);
}
void Program::set_vector(const std::string& name, const glm::vec3& vec) {
  set_value<glm::vec3>(locations, programID, name, vec);
}
void Program::set_vector(const std::string& name, const glm::vec2& vec) {
  set_value<glm::vec2>(locations, programID, name, vec);
}
void Program::set_vector(const std::string& name, const glm::vec4& vec) {
  set_value<glm::vec4>(locations, programID, name, vec);
}
bool Program::is_loaded() { return loaded; }
