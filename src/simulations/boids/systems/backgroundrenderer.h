#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include <engine/engine.h>
#include <engine/renderers/spriterenderer.h>
#include <engine/systems/system.h>

class BackgroundRenderer : public engine::RenderSystem {
 public:
  BackgroundRenderer();
  void render(entt::registry &registry) override;

 private:
  engine::SpriteRenderer sprite;
};

#endif  // BACKGROUNDRENDERER_H
