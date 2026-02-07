#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Image.h"
#include "GLTexture2D.h"

class Grid2D {
public:
  Grid2D(size_t width, size_t height);
  Grid2D(size_t width, size_t height, const std::vector<float> data);
  
  Grid2D(const Grid2D& other);
  Grid2D(const Image& image);
  
  Grid2D(std::istream &is);
  void save(std::ostream &os) const;

  size_t getWidth() const;
  size_t getHeight() const;
  std::string toString() const;
  std::vector<uint8_t> toByteArray() const;
  Grid2D toSignedDistance(float threshold) const;
  GLTexture2D toTexture() const;

  void setValue(size_t x, size_t y, float value);
  float getValueNormalized(float x, float y) const;
  float getValue(size_t x, size_t y) const;
  float sample(float x, float y) const ;
  float sample(const Vec2& pos) const ;
  
  Vec3 normal(float x, float y) const;
  Vec3 normal(const Vec2& pos) const;

  static Grid2D genRandom(size_t x, size_t y);
  static Grid2D genRandom(size_t x, size_t y, uint32_t seed);
  Grid2D operator*(const float& value) const;
  Grid2D operator/(const float& value) const;
  Grid2D operator+(const float& value) const;
  Grid2D operator-(const float& value) const;

  Grid2D operator+(const Grid2D& other) const;
  Grid2D operator/(const Grid2D& other) const;
  Grid2D operator*(const Grid2D& other) const;
  Grid2D operator-(const Grid2D& other) const;
  void normalize(const float maxVal = 1);

  Vec2t<size_t> maxValue() const;
  Vec2t<size_t> minValue() const;
  
  void fill(float value);

  friend std::ostream& operator<<(std::ostream &os, const Grid2D& v);

  static Grid2D fromBMP(const std::string& filename);
    
private:
  size_t width;
  size_t height;
  std::vector<float> data{};
  size_t index(size_t x, size_t y) const;
  
  std::pair<size_t,size_t> findMaxSize(const Grid2D& other) const;
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
