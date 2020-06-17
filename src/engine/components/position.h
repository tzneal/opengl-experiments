#ifndef POSITION_H
#define POSITION_H
#include <glm/glm.hpp>
namespace engine {
// wrapped types so we can differentiate with entt
struct Position {
  glm::vec2 Value;
};
struct Velocity {
  glm::vec2 Value;
};
struct Color {
  glm::vec4 Value;
};

}  // namespace engine
#endif  // POSITION_H
