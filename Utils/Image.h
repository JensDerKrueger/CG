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

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
