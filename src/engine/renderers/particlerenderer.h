#ifndef ENGINE_PARTICLERENDERER_H
#define ENGINE_PARTICLERENDERER_H

#include <engine/engine.h>
#include <engine/shader/program.h>
#include <engine/shader/texture.h>
#include <filesystem>
namespace engine {
class ParticleRenderer {
 public:
  explicit ParticleRenderer(const std::filesystem::path& texture);

  void bind();
  void unbind();

  void set_scale(float scale);
  void set_rotation(float scale);
  void set_color(const glm::vec4& vec);
  void render(const glm::vec2& offset);

 private:
  Texture texture;
  GLuint quadVAO;
};
}  // namespace engine
#endif  // ENGINE_PARTICLERENDERER_H
