#include <engine/engine.h>
#include <spdlog/spdlog.h>

#include "systems/backgroundrenderer.h"
#include "systems/boidrenderer.h"

int main() {
  engine::Engine eng;
  if (!eng.init()) {
    spdlog::error("error initializing engine");
    exit(1);
  }

  auto boids = std::make_shared<BoidRenderer>();
  eng.add_render_system(boids);
  eng.add_update_system(boids);
  eng.add_ui_system(boids);

  while (eng.running()) {
    eng.update();
    eng.render_frame();
    eng.poll();
  }
  return 0;
}
