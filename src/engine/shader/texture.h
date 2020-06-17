#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <filesystem>
namespace engine {
class Texture {
 public:
  Texture();
  Texture(const std::filesystem::path& path, bool generateAlpha = false);
  void load(const std::filesystem::path& path, bool generateAlpha = false);
  void bind();
  GLuint ID;
  // texture configuration
  GLuint Wrap_S{GL_CLAMP_TO_EDGE};  // wrapping mode on S axis
  GLuint Wrap_T{GL_CLAMP_TO_EDGE};  // wrapping mode on T axis
  GLuint Filter_Min{GL_LINEAR};     // filtering mode if texture pixels < screen pixels
  GLuint Filter_Max{GL_LINEAR};     // filtering mode if texture pixels > screen pixels
  int Width{-1}, Height{-1};
  void unbind();
};
}  // namespace engine

#endif  // TEXTURE_H
