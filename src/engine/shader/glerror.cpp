#include "glerror.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace engine {
const char *glErrorString(int err) {
  switch (err) {
    case GL_NO_ERROR:
      return "No Error";
    case GL_INVALID_ENUM:
      return "Invalid Enum";
    case GL_INVALID_OPERATION:
      return "Invalid Operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "Invalid Framebuffer Operation";
    case GL_OUT_OF_MEMORY:
      return "Out of Memory";
    case GL_STACK_UNDERFLOW:
      return "Stack Underflow";
    case GL_STACK_OVERFLOW:
      return "Stack Overflow";
    default:
      return "Unknown Error";
  }
}

void printProgramLog(int programID) {
  if (glIsProgram(programID)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<char> infoLog(maxLength, 0x00);

    // Get info log
    glGetProgramInfoLog(programID, maxLength, &infoLogLength, &infoLog[0]);
    if (infoLogLength > 0) {
      spdlog::warn("{}", &infoLog[0]);
    }
  } else {
    spdlog::warn("{} is not a program", programID);
  }
}

void printShaderLog(int shaderID) {
  if (glIsShader(shaderID)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<char> infoLog(maxLength, 0x00);

    // Get info log
    glGetShaderInfoLog(shaderID, maxLength, &infoLogLength, &infoLog[0]);
    if (infoLogLength > 0) {
      spdlog::warn("{}", &infoLog[0]);
    }
  } else {
    spdlog::warn("{} is not a program", shaderID);
  }
}
}  // namespace engine