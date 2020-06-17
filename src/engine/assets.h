#ifndef ASSETS_H
#define ASSETS_H
#include <filesystem>

namespace engine {
std::filesystem::path resolve_asset_path(const std::filesystem::path& path);
std::vector<char> read_file(const std::filesystem::path& path);
struct Image {
  Image(int width, int height, int nChannels, unsigned char* pixels);
  ~Image();
  int Width;
  int Height;
  int NChannels;
  unsigned char* Pixels;
};
std::unique_ptr<Image> read_image(const std::filesystem::path& path);
}  // namespace engine
#endif  // ASSETS_H
