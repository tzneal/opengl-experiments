#include "backgroundrenderer.h"

#include <engine/components/position.h>
#include <glm/glm.hpp>

namespace {
struct Ball {};
}  // namespace

BackgroundRenderer::BackgroundRenderer() : sprite("textures/transparent.png") {}

void BackgroundRenderer::render(entt::registry &registry, const glm::vec2 &screen_size) {
  sprite.bind();
  auto view = registry.view<engine::Position, Ball>();
  while (view.size() != 10) {
    auto ball = registry.create();
    registry.emplace<engine::Position>(ball, engine::Position{{5.0f, 10.0f}});
    registry.emplace<Ball>(ball);
  }

  glm::vec2 size{50, 50};
  float rotate = 0;
  glm::vec3 color{1, 1, 1};
  sprite.set_color(color);

  for (auto &ball : view) {
    auto &position = view.get<engine::Position>(ball);
    for (int i = 0; i < 10; i++) {
      sprite.render(glm::vec2{position.Value.x + 25 * i, position.Value.y}, size, rotate);
    }
  }
}
