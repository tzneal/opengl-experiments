#include "assets.h"
#include <spdlog/spdlog.h>
#include <stb/stb_image.h>
#include <fstream>

namespace engine {
std::filesystem::path resolve_asset_path(const std::filesystem::path& path) {
  std::filesystem::path dir = std::filesystem::current_path();
  while (!std::filesystem::exists(dir / "assets")) {
    dir = dir.parent_path();
    if (dir.parent_path() == dir) {
      break;
    }
  }
  auto fullPath = dir / "assets" / path;
  if (!std::filesystem::exists(fullPath)) {
    spdlog::error("unable to resolve asset {}", path.c_str());
  }
  return fullPath;
}

std::vector<char> read_file(const std::filesystem::path& path) {
  auto fullPath = resolve_asset_path(path);
  std::ifstream file(fullPath, std::ios::binary);
  std::vector<char> ret;
  std::istreambuf_iterator<char> begin(file), end;
  std::copy(begin, end, std::back_inserter(ret));
  return ret;
}

std::unique_ptr<Image> read_image(const std::filesystem::path& path) {
  int width, height, nrChannels;
  auto fullPath = resolve_asset_path(path);
  unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
  return std::make_unique<Image>(width, height, nrChannels, data);
}

Image::Image(int width, int height, int nChannels, unsigned char* pixels) {
  this->Width = width;
  this->Height = height;
  this->NChannels = nChannels;
  this->Pixels = pixels;
}
Image::~Image() {
  stbi_image_free(this->Pixels);
  Pixels = 0;
  Width = -1;
  Height = -1;
  NChannels = -1;
}
}  // namespace engine