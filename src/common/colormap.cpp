#include "colormap.h"

#include <algorithm>

#include "lerp.h"

#include <spdlog/spdlog.h>
#include <glm/ext.hpp>

using namespace common;

void ColorMap::add_color(float f, const glm::vec3& color) {
  colors.push_back(std::make_pair(f, color));
  std::sort(colors.begin(), colors.end(),
            [](const std::pair<float, glm::vec3>& lhs,
               const std::pair<float, glm::vec3>& rhs) -> bool { return lhs.first < rhs.first; });
}
bool ColorMap::is_valid() const { return colors.size() > 1; }

glm::vec3 ColorMap::operator()(float v) const {
  // limit to our range
  if (v < colors[0].first) {
    return colors[0].second;
  }
  if (v > colors.back().first) {
    return colors.back().second;
  }

  size_t beg = 0;
  size_t end = 0;
  for (size_t i = 0; i < colors.size() - 1; i++) {
    if (v >= colors[i].first && v <= colors[i + 1].first) {
      beg = i;
      end = i + 1;
      break;
    }
  }

  Lerp<float> lerp{colors[beg].first, 0, colors[end].first, 1.0};
  return glm::mix(colors[beg].second, colors[end].second, lerp(v));
}
