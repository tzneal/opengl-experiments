#ifndef BOIDRENDERER_H
#define BOIDRENDERER_H

#include <engine/systems/system.h>
#include <glm/ext.hpp>
#include <random>
class BoidRenderer : public engine::NVGSystem,
                     public engine::UpdateSystem,
                     public engine::UISystem {
 public:
  void render(entt::registry &registry, const glm::vec2 &screen_size, NVGcontext *nvg) override;
  void update(entt::registry &registry, const glm::vec2 &screen_size, float dt) override;
  void render_ui(entt::registry &registry) override;

 private:
  int num_boids{10};
  int size{20};
  float min_distance{20};
  float max_velocity{10};
  std::mt19937 gen;
  bool reset{false};
};

#endif  // BOIDRENDERER_H
