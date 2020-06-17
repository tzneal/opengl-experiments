#include "particlerenderer.h"

#include <engine/engine.h>
#include <engine/shader/program.h>
#include <spdlog/spdlog.h>
#include <glm/ext.hpp>

using namespace engine;
namespace {
Program particleProg("shaders/particle-vertex.glsl", "shaders/particle-fragment.glsl");
void on_window_changed(int width, int height) {
  if (particleProg.is_loaded()) {
    glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    particleProg.bind();
    particleProg.set_matrix("projection", projection);
    particleProg.set_float("scale", 10.0f);
  }
}
bool inited = []() -> bool {
  on_glcontext_created.connect(&Program::load, &particleProg);
  on_window_resized.connect(&on_window_changed);
  return true;
}();

}  // namespace

ParticleRenderer::ParticleRenderer(const std::filesystem::path& texturePath) {
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
void ParticleRenderer::bind() {
  particleProg.bind();
  glActiveTexture(GL_TEXTURE0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  texture.bind();
}

void ParticleRenderer::unbind() {
  texture.unbind();
  particleProg.unbind();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleRenderer::render(const glm::vec2& offset) {
  particleProg.set_vector("offset", offset);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void ParticleRenderer::set_color(const glm::vec4& vec) { particleProg.set_vector("color", vec); }
void ParticleRenderer::set_scale(float scale) { particleProg.set_float("scale", scale); }
void ParticleRenderer::set_rotation(float scale) { particleProg.set_float("rotation", scale); }
