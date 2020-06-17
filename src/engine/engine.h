#ifndef ENGINE_H
#define ENGINE_H
#include "engine/systems/system.h"

#include <filesystem>
#include <string>

#include <SDL2/SDL_system.h>
#include <SDL2/SDL_video.h>
#include <nanovg.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <sigslot/signal.hpp>

namespace engine {
struct EngineInitParams {
  int Width{640};
  int Height{480};
  std::string WindowTitle{"engine"};
};

extern sigslot::signal<> on_glcontext_created;
extern sigslot::signal<int, int> on_window_resized;

class Engine {
 public:
  bool init(const EngineInitParams &params = EngineInitParams{});
  ~Engine();

  void render_frame();
  bool running();
  void poll();
  void set_clear_color(const glm::vec3 &color);

  void add_ui_system(std::shared_ptr<UISystem> &&system);
  void add_update_system(std::shared_ptr<UpdateSystem> &&system);
  void add_render_system(std::shared_ptr<RenderSystem> &&system);
  entt::entity create_entity();
  void update();
  entt::registry &get_registry();

 private:
  bool init_video(const EngineInitParams &params);
  bool init_imgui();
  bool init_opengl();
  bool init_openglcontext(const EngineInitParams initParams);
  bool init_openglmatrices(const EngineInitParams &params);
  bool init_nanovg();

 private:
  SDL_Window *window{0};
  bool shutdown{false};
  entt::registry registry;
  std::vector<std::shared_ptr<UISystem>> ui_systems;
  std::vector<std::shared_ptr<UpdateSystem>> update_systems;
  std::vector<std::shared_ptr<RenderSystem>> render_systems;
  bool debug{true};
  EngineInitParams params;
  SDL_GLContext gl_context;
  std::string glsl_version;
  Uint32 previousTicks{0};
  NVGcontext *nvg{0};
  glm::vec3 clear_color{0, 0, 0};
};
}  // namespace engine
#endif