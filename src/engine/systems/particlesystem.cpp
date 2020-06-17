#include "particlesystem.h"
#include <common/lerp.h>
#include <engine/components/position.h>
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
  }
}
bool inited = []() -> bool {
  on_glcontext_created.connect(&Program::load, &particleProg);
  on_window_resized.connect(&on_window_changed);
  return true;
}();

}  // namespace

ParticleSystem::ParticleSystem() {}
void ParticleSystem::render(entt::registry& registry, const glm::vec2& screen_size) {
  particleProg.bind();

  bool additiveBlending = false;
  for (auto& em : emitters) {
    em.second.texture.bind();
    if (!additiveBlending && em.second.emitter.AdditiveBlending) {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      additiveBlending = true;
    }
    glBindVertexArray(em.second.vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, em.second.emitter.NumParticles);
  }
  glBindVertexArray(0);
  if (additiveBlending) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}
void ParticleSystem::update(entt::registry& registry, const glm::vec2& screen_size, float dt) {
  for (auto& empair : emitters) {
    auto& em = empair.second;

    bool hasColormap = em.emitter.ColorMap.is_valid();
    common::Lerp<float> lifetime_to_alpha{0.0f, 0.0f, em.emitter.Lifetime, 1.0f};
    for (int i = 0; i < em.emitter.NumParticles; i++) {
      em.positions[i] += dt * em.velocities[i];
      em.rotations[i] += 0.01;
      em.lifetimes[i] -= dt;

      if (em.emitter.Fade) {
        em.alphas[i] = lifetime_to_alpha(em.lifetimes[i]);
      }
      if (hasColormap) {
        em.colors[i] = em.emitter.ColorMap(em.lifetimes[i]);
      }

      // particle is dead
      if (em.lifetimes[i] < 0) {
        if (em.emitter.Respawn) {
          // respawn the particle
          em.positions[i] = glm::vec2{em.emitter.X, em.emitter.Y};
          em.lifetimes[i] = em.lifetime_dist(gen);
          em.rotations[i] = em.rotation_dist(gen);
          em.velocities[i] = glm::vec2{em.velocity_dist(gen), em.velocity_dist(gen)};
        } else {
          em.alphas[i] = 0.0;
        }
      }
    }

    // rebind position data
    glBindBuffer(GL_ARRAY_BUFFER, empair.second.position_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * empair.second.emitter.NumParticles,
                    &empair.second.positions[0]);

    // rotation
    glBindBuffer(GL_ARRAY_BUFFER, empair.second.rotation_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * empair.second.emitter.NumParticles,
                    &empair.second.rotations[0]);

    if (hasColormap) {
      glBindBuffer(GL_ARRAY_BUFFER, empair.second.color_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * empair.second.emitter.NumParticles,
                      &empair.second.colors[0]);
    }
    // alpha only changes if fading
    if (em.emitter.Fade) {
      glBindBuffer(GL_ARRAY_BUFFER, empair.second.alpha_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * empair.second.emitter.NumParticles,
                      &empair.second.alphas[0]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

int ParticleSystem::add_emitter(const engine::ParticleEmitter& emitter) {
  int id = next_emitter_id++;

  ParticleEmitterInfo pinfo{emitter, gen};
  pinfo.emitter = emitter;
  pinfo.texture = Texture{emitter.Texture, true};

  for (auto& pos : pinfo.positions) {
    pos.x = emitter.X;
    pos.y = emitter.Y;
  }

  // store instance data in an array buffer
  // --------------------------------------
  glGenBuffers(1, &pinfo.position_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, pinfo.position_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * emitter.NumParticles, &pinfo.positions[0],
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  float quadVertices[] = {
      // positions and texture coords
      0.0f, 1.0f,  //
      1.0f, 0.0f,  //
      0.0f, 0.0f,  //
      0.0f, 1.0f,  //
      1.0f, 0.0f,  //
      1.0f, 1.0f,  //
  };
  unsigned int quadVBO;
  const int VERTEX_LOC = 0;
  // vertices
  glGenVertexArrays(1, &pinfo.vao);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(pinfo.vao);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(VERTEX_LOC);
  glVertexAttribPointer(VERTEX_LOC, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

  // position
  const int POSITION_LOC = 1;
  glEnableVertexAttribArray(POSITION_LOC);
  glBindBuffer(GL_ARRAY_BUFFER,
               pinfo.position_vbo);  // this attribute comes from a different vertex buffer
  glVertexAttribPointer(POSITION_LOC, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(POSITION_LOC, 1);  // tell OpenGL this is an instanced vertex attribute.

  // TODD STUFF
  // rotations
  const int ROT_LOC = 2;
  glGenBuffers(1, &pinfo.rotation_vbo);
  glBindVertexArray(pinfo.vao);
  glBindBuffer(GL_ARRAY_BUFFER, pinfo.rotation_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * emitter.NumParticles, &pinfo.rotations[0],
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(ROT_LOC);
  glVertexAttribPointer(ROT_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
  glVertexAttribDivisor(ROT_LOC, 1);

  // scale
  const int SCALE_LOC = 3;
  glGenBuffers(1, &pinfo.scale_vbo);
  glBindVertexArray(pinfo.vao);
  glBindBuffer(GL_ARRAY_BUFFER, pinfo.scale_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * emitter.NumParticles, &pinfo.scales[0],
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(SCALE_LOC);
  glVertexAttribPointer(SCALE_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
  glVertexAttribDivisor(SCALE_LOC, 1);

  // color
  const int COLOR_LOC = 4;
  glGenBuffers(1, &pinfo.color_vbo);
  glBindVertexArray(pinfo.vao);
  glBindBuffer(GL_ARRAY_BUFFER, pinfo.color_vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * emitter.NumParticles, &pinfo.colors[0],
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(COLOR_LOC);
  glVertexAttribPointer(COLOR_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glVertexAttribDivisor(COLOR_LOC, 1);

  // alpha
  const int ALPHA_LOC = 5;
  glGenBuffers(1, &pinfo.alpha_vbo);
  glBindVertexArray(pinfo.vao);
  glBindBuffer(GL_ARRAY_BUFFER, pinfo.alpha_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * emitter.NumParticles, &pinfo.alphas[0],
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(ALPHA_LOC);
  glVertexAttribPointer(ALPHA_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
  glVertexAttribDivisor(ALPHA_LOC, 1);

  emitters.emplace(id, std::move(pinfo));

  return id;
}
ParticleSystem::ParticleEmitterInfo::ParticleEmitterInfo(ParticleEmitter em, std::mt19937& gen)
    : lifetime_dist{0, em.Lifetime},
      velocity_dist{0, 20},
      rotation_dist{0, 2 * M_PI},
      scale_dist{5, 50} {
  positions.resize(em.NumParticles);
  velocities.resize(em.NumParticles);
  for (auto& vel : velocities) {
    vel = glm::vec2{velocity_dist(gen), velocity_dist(gen)};
  }
  rotations.resize(em.NumParticles);
  for (auto& rot : rotations) {
    rot = rotation_dist(gen);
  }

  scales.resize(em.NumParticles);
  for (auto& scale : scales) {
    scale = scale_dist(gen);
  }

  colors.resize(em.NumParticles);
  for (auto& color : colors) {
    color = glm::vec3{1.0f, 0.0f, 0.0f};
  }

  lifetimes.resize(em.NumParticles);
  for (auto& lifetime : lifetimes) {
    lifetime = lifetime_dist(gen);
  }

  alphas.resize(em.NumParticles, 1.0f);
  if (em.Fade) {
    common::Lerp<float> lifetime_to_alpha{0.0f, 0.0f, em.Lifetime, 1.0f};
    for (int i = 0; i < em.NumParticles; i++) {
      alphas[i] = lifetime_to_alpha(lifetimes[i]);
    }
  }
}
