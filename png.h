#pragma once

#include <cstdint>
#include <string>

#include <Image.h>

namespace PNG {
  
  bool save(const std::string& filePath, const Image& image,
            bool writeSRGBChunk = false, uint8_t srgbRenderingIntent = 0);
}
