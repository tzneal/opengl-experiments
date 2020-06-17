#ifndef LERP_H
#define LERP_H
#include <algorithm>

namespace common {
template <typename T>
class Lerp {
 public:
  Lerp() {}
  Lerp(T x1, T y1, T x2, T y2, bool clamped = false) : clamped{clamped} {
    m = (y2 - y1) / (x2 - x1);
    b = y1 - m * x1;
    minX = std::min(x1, x2);
    maxX = std::max(x1, x2);
    minY = std::min(y1, y2);
    maxY = std::max(y1, y2);
  }
  T operator()(T x) const {
    auto v = m * x + b;
    if (clamped) {
      return std::clamp(v, minY, maxY);
    }
    return v;
  }
  T inverse(T y) const {
    auto v = (y - b) / m;
    if (clamped) {
      return std::clamp(v, minX, maxX);
    }
    return v;
  }

 private:
  T m = 0;
  T b = 0;
  T minX{}, minY{}, maxX{}, maxY{};
  bool clamped;
};
}  // namespace common
#endif
