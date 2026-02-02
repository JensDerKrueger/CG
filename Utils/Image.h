#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "Vec4.h"

class Grid2D;

class Image {
public:
  uint32_t width;
  uint32_t height;
  uint8_t componentCount;
  std::vector<uint8_t> data;

  Image(const Vec4& color);
  
  Image(uint32_t width = 100,
        uint32_t height = 100,
        uint8_t componentCount = 4);

  Image(uint32_t width,
        uint32_t height,
        uint8_t componentCount,
        std::vector<uint8_t> data);
  
  void multiply(const Vec4& color);
  void generateAlpha(uint8_t alpha=255);
  void generateAlphaFromLuminance();
  size_t computeIndex(uint32_t x, uint32_t y, uint8_t component) const;
  uint8_t getValue(uint32_t x, uint32_t y, uint8_t component) const;
  uint8_t sample(float x, float y, uint8_t component) const;
  uint8_t getLumiValue(uint32_t x, uint32_t y) const;
  void setValue(uint32_t x, uint32_t y, uint8_t component, uint8_t value);
  void setValue(uint32_t x, uint32_t y, uint8_t value);
  void setNormalizedValue(uint32_t x, uint32_t y, float value);
  void setNormalizedValue(uint32_t x, uint32_t y, uint8_t component, float value);
  std::string toCode(const std::string& varName="myImage", bool padding=false) const;
  std::string toASCIIArt(bool bSmallTable=true) const;
  Image filter(const Grid2D& filter) const;
  Image toGrayscale() const;

  static Image genTestImage(uint32_t width,
                            uint32_t height);

  Image crop(uint32_t blX, uint32_t blY, uint32_t trX, uint32_t trY) const;
  Image resample(uint32_t newWidth) const;
  Image cropToAspectAndResample(uint32_t newWidth, uint32_t newHeight) const;
  Image flipVertical() const;
  Image flipHorizontal() const;

private:
  uint8_t linear(uint8_t a, uint8_t b, float alpha) const;
};
