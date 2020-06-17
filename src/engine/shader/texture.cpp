#include "texture.h"
#include <engine/assets.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

using namespace engine;
Texture::Texture() {}
Texture::Texture(const std::filesystem::path& path, bool generateAlpha) {
  load(path, generateAlpha);
}

void Texture::load(const std::filesystem::path& path, bool generateAlpha) {
  glGenTextures(1, &ID);
  auto img = read_image(path);
  this->Width = img->Width;
  this->Height = img->Width;
  // create Texture
  glBindTexture(GL_TEXTURE_2D, this->ID);
  if (img->NChannels == 3) {
    if (generateAlpha) {
      std::vector<uint8_t> pixels;
      pixels.resize(img->Width * img->Height * 4);
      for (int y = 0; y < img->Height; y++) {
        for (int x = 0; x < img->Width; x++) {
        }
      }
      spdlog::error("implement this");
      exit(0);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->Width, img->Height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 img->Pixels);
  } else {
    if (generateAlpha) {
      for (int y = 0; y < img->Height; y++) {
        for (int x = 0; x < img->Width; x++) {
          int offset = (x + y * img->Width) * 4;
          glm::vec<4, uint8_t>& pixel =
              *reinterpret_cast<glm::vec<4, uint8_t>*>(img->Pixels + offset);
          pixel.a = static_cast<uint8_t>((static_cast<int>(pixel.r) + pixel.g + pixel.b) / 3);
        }
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->Width, img->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 img->Pixels);
  }

  // set Texture wrap and filter modes
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, ID); }

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
