#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "bmp.h"
#include "Vec2.h"
#include "GLTexture2D.h"
#include "GLArray.h"
#include "GLBuffer.h"
#include "Grid2D.h"

struct CharPosition {
  char c;
  Vec2ui topLeft;
  Vec2ui bottomRight;
};

struct CharTex {
  GLTexture2D tex;
  Mat4 scale;
  Mat4 trans;
  float width;
  float height;
};

enum class Alignment {
  Left,
  Right,
  Center
};

class FontEngine {
public:
  FontEngine();
  virtual ~FontEngine() {}
  void render(const std::string& text, float winAspect, float height,
              const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});
  void renderFixedWidth(const std::string& text, float winAspect, float width,
                        const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});

  void render(uint32_t number, float winAspect, float height,
              const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});
  void renderFixedWidth(uint32_t number, float winAspect, float width,
                        const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});

  Vec2 getSize(const std::string& text, float winAspect, float height) const;
  Vec2 getSizeFixedWidth(const std::string& text, float winAspect, float width) const;

  Vec2 getSize(uint32_t number, float winAspect, float height) const;
  Vec2 getSizeFixedWidth(uint32_t number, float winAspect, float width) const;

  std::string getAllCharsString() const;

  std::map<char,CharTex> chars;
  std::map<char,CharTex> sdChars;
  
  void setRenderAsSignedDistanceField(bool renderAsSignedDistanceField) {
    this->renderAsSignedDistanceField = renderAsSignedDistanceField;
  }

  bool getRenderAsSignedDistanceField() const {
    return renderAsSignedDistanceField;
  }

private:
  GLProgram simpleProg;
  GLProgram simpleDistProg;
  GLArray   simpleArray;
  GLBuffer  simpleVb;
  bool renderAsSignedDistanceField;

};

class FontRenderer {
public:
  FontRenderer(const std::string& imageFilename,
               const std::string& positionFilename);

  FontRenderer(const Image& fontImage,
               const std::string& positionFilename);

  FontRenderer(const Image& fontImage,
               const std::vector<CharPosition>& positions);

  Image render(const std::string& text) const;
  Image render(uint32_t number) const;
  
  static const std::vector<CharPosition> loadPositions(const std::string& positionFilename);

  std::string toCode(const std::string& varName) const;

  std::shared_ptr<FontEngine> generateFontEngine() const;
  
private:
  Image fontImage;
  std::vector<CharPosition> positions;
  
  const CharPosition& findElement(char c) const;
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
