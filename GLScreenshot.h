#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <algorithm>

#include "GLApp.h"

#include "png.h"
#include "bmp.h"

class GLScreenshot {
public:
  static bool save(const std::string &fileName) {
    std::filesystem::path p(fileName);
    if (!p.has_extension()) return false;

    std::string ext = p.extension().string(); // includes '.'
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return char(std::tolower(c)); });

    if (ext == ".png") return savePng(fileName);
    if (ext == ".ppm") return savePpm(fileName);
    if (ext == ".bmp" || ext == ".bpm") return saveBmp(fileName);

    return false;
  }

  static bool savePpm(const std::string &fileName) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width  = viewport[2];
    int height = viewport[3];

    if (width <= 0 || height <= 0) return false;

    std::vector<unsigned char> pixels(size_t(width * height * 3));

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream out(fileName, std::ios::binary);
    if (!out) return false;

    out << "P6\n" << width << " " << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y) {
      const unsigned char *row =
      pixels.data() + static_cast<std::size_t>(y) * size_t(width * 3);
      out.write(reinterpret_cast<const char *>(row),
                static_cast<std::streamsize>(width * 3));
    }

    return static_cast<bool>(out);
  }

  static bool savePng(const std::string &fileName) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width  = viewport[2];
    int height = viewport[3];

    if (width <= 0 || height <= 0) return false;

    Image image{uint32_t(width),uint32_t(height),3};

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,
                 0,
                 width,
                 height,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image.data.data());

    return PNG::save(fileName, image);
  }

  static bool saveBmp(const std::string &fileName) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width  = viewport[2];
    int height = viewport[3];

    if (width <= 0 || height <= 0) return false;

    Image image{uint32_t(width),uint32_t(height),3};

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,
                 0,
                 width,
                 height,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image.data.data());

    return BMP::save(fileName, image);
  }
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
