#include "boidrenderer.h"

#include <engine/components/position.h>
#include <imgui/imgui.h>
#include <spdlog/spdlog.h>
#include <glm/gtx/component_wise.hpp>

namespace {
struct Boid {
  float rotation{0};
};
glm::vec2 limit_mag(const glm::vec2 &vec, float v) {
  if (glm::compAdd(glm::abs(vec)) <= v) {
    return vec;
  }
  return vec / glm::compAdd(glm::abs(vec)) * v;
}
}  // namespace

void BoidRenderer::render(entt::registry &registry, NVGcontext *nvg) {
  static const NVGpaint paint =
      nvgLinearGradient(nvg, 0, 0, size, 0, nvgRGBf(0, 0, 0), nvgRGBf(1, 0, 0));
  auto view = registry.view<engine::Position, engine::Velocity, Boid>();
  view.each([nvg, this](auto &pos, auto &vel, auto &boid) {
    nvgSave(nvg);
    nvgBeginPath(nvg);
    nvgTranslate(nvg, pos.Value.x, pos.Value.y);
    auto angle = atan2(vel.Value.y, vel.Value.x);
    nvgRotate(nvg, angle);
    nvgMoveTo(nvg, 0, -size / 2);
    nvgLineTo(nvg, 0, size / 2);
    nvgLineTo(nvg, size, 0);
    // nvgFillColor(nvg, nvgRGBA(255, 192, 0, 255));
    nvgFillPaint(nvg, paint);
    nvgFill(nvg);
    nvgRestore(nvg);
  });
}

void BoidRenderer::update(entt::registry &registry, float dt) {
  auto view = registry.view<engine::Position, engine::Velocity, Boid>();
  while (view.size() > num_boids) {
    auto beg = view.begin();
    for (int i = 0; i < num_boids; i++) {
      beg++;
    }
    registry.destroy(beg, view.end());
  }
  std::uniform_real_distribution<float> pos_x{0, 500}, pos_y{0, 500};
  std::normal_distribution<float> vel_dist{0, max_velocity};
  while (view.size() < num_boids) {
    auto ent = registry.create();
    registry.emplace<engine::Position>(ent, engine::Position{{pos_x(gen), pos_y(gen)}});
    registry.emplace<engine::Velocity>(ent, engine::Velocity{{vel_dist(gen), vel_dist(gen)}});
    registry.emplace<Boid>(ent, Boid{});
  }

  if (reset) {
    view.each([this, &pos_x, &pos_y, &vel_dist](auto &pos, auto &vel, auto &boid) {
      pos.Value.x = pos_x(gen);
      pos.Value.y = pos_y(gen);
      vel.Value.x = vel_dist(gen);
      vel.Value.y = vel_dist(gen);
    });
    reset = false;
    return;
  }
  // nothing to do
  if (num_boids == 0) {
    return;
  }

  for (auto i = view.begin(); i != view.end(); i++) {
    auto &pos = view.get<engine::Position>(*i);
    auto &vel = view.get<engine::Velocity>(*i);

    glm::vec2 avoid_collisions{0, 0};
    glm::vec2 align{0, 0};
    glm::vec2 approach{0, 0};
    int nNearby = 0;

    for (auto j = view.begin(); j != view.end(); j++) {
      if (i == j) {
        continue;
      }
      auto &pos2 = view.get<engine::Position>(*j);
      auto &vel2 = view.get<engine::Velocity>(*j);
      auto distance = glm::distance(pos.Value, pos2.Value);
      if (distance > 10 * min_distance) {
        // too far away to care
        continue;
      }
      nNearby++;
      align += (vel.Value - vel2.Value);
      approach += (pos.Value - pos2.Value);
      if (distance < min_distance) {
        avoid_collisions += (pos.Value - pos2.Value) / distance;
      }
    }

    if (nNearby > 0) {
      align /= static_cast<float>(nNearby);
      vel.Value += limit_mag(avoid_collisions, 5.0f);
      vel.Value -= limit_mag(align, 1.0f);
      vel.Value -= limit_mag(approach, 1.0f);
    }

    vel.Value = limit_mag(vel.Value, max_velocity);

    int SCREEN_WIDTH = 500;
    pos.Value += vel.Value * dt;
    if (pos.Value.x < 0 || pos.Value.x > SCREEN_WIDTH) {
      vel.Value.x *= -1;
      pos.Value.x += vel.Value.x * dt * 2;
    }
    if (pos.Value.y < 0 || pos.Value.y > SCREEN_WIDTH) {
      vel.Value.y *= -1;
      pos.Value.y += vel.Value.y * dt * 2;
    }
    // pos.Value = glm::clamp(pos.Value, glm::vec2{0, 0}, glm::vec2{SCREEN_WIDTH, SCREEN_WIDTH});
  }
}
void BoidRenderer::render_ui(entt::registry &registry) {
  if (ImGui::Begin("Boids")) {
    ImGui::SliderInt("Number", &num_boids, 0, 500);
    ImGui::SliderInt("Size", &size, 10, 50);
    ImGui::SliderFloat("Min Distance", &min_distance, 0, 250);
    ImGui::SliderFloat("Max Velocity", &max_velocity, 0, 100);
    if (ImGui::Button("Reset")) {
      reset = true;
    }
  }
  ImGui::End();
}
