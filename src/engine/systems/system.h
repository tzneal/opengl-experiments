#ifndef SYSTEM_H
#define SYSTEM_H

#include <SDL_system.h>
#include <nanovg.h>
#include <entt/entt.hpp>

namespace engine {
class RenderSystem {
 public:
  virtual ~RenderSystem() = default;
  virtual void render(entt::registry &registry) = 0;
};
class NVGSystem : public RenderSystem {
 public:
  void set_context(NVGcontext *nvg);
  void render(entt::registry &registry) override;
  virtual void render(entt::registry &registry, NVGcontext *nvg) = 0;

 private:
  NVGcontext *nvg{0};
};
class UISystem {
 public:
  virtual ~UISystem() = default;
  virtual void render_ui(entt::registry &registry) = 0;
};
class System {
 public:
  virtual ~System() = default;
  virtual void update(entt::registry &registry, float dt) = 0;
};

}  // namespace engine
#endif  // SYSTEM_H
