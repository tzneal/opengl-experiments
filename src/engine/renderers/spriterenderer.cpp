#include "spriterenderer.h"
#include <engine/engine.h>
#include <spdlog/spdlog.h>
#include <glm/ext.hpp>
using namespace engine;
namespace {
Program spriteProg("shaders/sprite-vertex.glsl", "shaders/sprite-fragment.glsl");
void on_window_changed(int width, int height) {
  if (spriteProg.is_loaded()) {
    glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    spriteProg.bind();
    spriteProg.set_matrix("projection", projection);
  }
}
bool inited = []() -> bool {
  on_glcontext_created.connect(&Program::load, &spriteProg);
  on_window_resized.connect(&on_window_changed);
  return true;
}();

}  // namespace

SpriteRenderer::SpriteRenderer(const std::filesystem::path& texturePath) {
  texture.load(texturePath);

  // configure VAO/VBO
  unsigned int VBO;
  float vertices[] = {// pos      // tex
                      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(quadVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void SpriteRenderer::bind() {
  spriteProg.bind();
  glActiveTexture(GL_TEXTURE0);
  texture.bind();
}

void SpriteRenderer::unbind() {
  texture.unbind();
  spriteProg.unbind();
}
void SpriteRenderer::render(const glm::vec2& position, const glm::vec2& size, float rotation) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));
  model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
  model = glm::scale(model, glm::vec3(size, 1.0f));
  render(model);
}

void SpriteRenderer::render(const glm::mat4& model) {
  spriteProg.set_matrix("model", model);

  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void SpriteRenderer::set_color(const glm::vec3& vec) { spriteProg.set_vector("spriteColor", vec); }
