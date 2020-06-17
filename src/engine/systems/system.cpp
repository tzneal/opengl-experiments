#include "system.h"
using namespace engine;
void NVGSystem::set_context(NVGcontext *nvg) { this->nvg = nvg; }
void NVGSystem::render(entt::registry &registry) { render(registry, nvg); }
