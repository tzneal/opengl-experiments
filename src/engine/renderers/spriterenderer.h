#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <engine/engine.h>
#include <engine/shader/program.h>
#include <engine/shader/texture.h>
#include <filesystem>
namespace engine {
class SpriteRenderer {
 public:
  explicit SpriteRenderer(const std::filesystem::path& texture);

  void bind();
  void unbind();

  void render(const glm::vec2& position, const glm::vec2& size, float rotation);
  void render(const glm::mat4& model);
  void set_color(const glm::vec3& vec);

 private:
  Texture texture;
  GLuint quadVAO;
};
}  // namespace engine
#endif  // SPRITERENDERER_H
