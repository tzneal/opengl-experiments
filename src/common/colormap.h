#ifndef COLORMAP_H
#define COLORMAP_H

#include <glm/vec3.hpp>
#include <vector>
namespace common {
class ColorMap {
 public:
  void add_color(float f, const glm::vec3& color);
  bool is_valid() const;
  glm::vec3 operator()(float v) const;

 private:
  std::vector<std::pair<float, glm::vec3>> colors;
};
}  // namespace common
#endif  // COLORMAP_H
