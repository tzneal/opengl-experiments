#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <random>

#include <common/colormap.h>
#include <engine/renderers/particlerenderer.h>
#include <unordered_map>
#include "system.h"

namespace engine {
// ParticleEmitter describes the particle emitter
struct ParticleEmitter {
  std::filesystem::path Texture{"textures/particle.png"};
  bool AdditiveBlending{false};
  int NumParticles{25};
  float X{300};
  float Y{300};
  float Lifetime{5.0f};
  bool Respawn{true};
  bool Fade{true};
  common::ColorMap ColorMap;
};

class ParticleSystem : public RenderSystem, public UpdateSystem {
 public:
  explicit ParticleSystem();
  void render(entt::registry &registry, const glm::vec2 &screen_size) override;
  void update(entt::registry &registry, const glm::vec2 &screen_size, float dt) override;

  int add_emitter(const ParticleEmitter &emitter);

 private:
  struct ParticleEmitterInfo {
    ParticleEmitterInfo(ParticleEmitter numParticles, std::mt19937 &twisterEngine);
    ParticleEmitter emitter;
    GLuint vao{0};
    GLuint position_vbo{0};
    GLuint rotation_vbo{0};
    GLuint scale_vbo{0};
    GLuint color_vbo{0};
    GLuint alpha_vbo{0};
    std::vector<glm::vec2> positions;
    std::vector<glm::vec2> velocities;
    std::vector<float> rotations;
    std::vector<float> scales;
    std::vector<float> alphas;
    std::vector<glm::vec3> colors;

    std::vector<float> lifetimes;
    Texture texture;

    std::uniform_real_distribution<float> lifetime_dist;
    std::normal_distribution<float> velocity_dist;
    std::uniform_real_distribution<float> rotation_dist;
    std::uniform_real_distribution<float> scale_dist;
  };

  std::mt19937 gen;
  int next_emitter_id{0};
  std::unordered_map<int, ParticleEmitterInfo> emitters;
};
}  // namespace engine
#endif  // PARTICLESYSTEM_H
