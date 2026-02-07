#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

#include "GLApp.h"

class GLScreenshot {
public:
  static bool savePpm(const std::string &fileName)
  {
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

  static bool saveBmp(const std::string &fileName)
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width  = viewport[2];
    int height = viewport[3];

    if (width <= 0 || height <= 0) return false;

    std::vector<unsigned char> pixels(size_t(width * height * 3));

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    const std::uint32_t headerSize = 54;
    const std::uint32_t rowStride  = static_cast<std::uint32_t>(width) * 3;
    const std::uint32_t padding    = (4 - (rowStride % 4)) % 4;
    const std::uint32_t rowSize    = rowStride + padding;
    const std::uint32_t dataSize   = rowSize * static_cast<std::uint32_t>(height);
    const std::uint32_t fileSize   = headerSize + dataSize;

    std::ofstream out(fileName, std::ios::binary);
    if (!out) return false;

    auto writeUint16 = [&out](std::uint16_t v) {
      out.put(static_cast<char>(v & 0xFF));
      out.put(static_cast<char>((v >> 8) & 0xFF));
    };

    auto writeUint32 = [&out](std::uint32_t v) {
      out.put(static_cast<char>(v & 0xFF));
      out.put(static_cast<char>((v >> 8) & 0xFF));
      out.put(static_cast<char>((v >> 16) & 0xFF));
      out.put(static_cast<char>((v >> 24) & 0xFF));
    };

    out.put('B');
    out.put('M');
    writeUint32(fileSize);
    writeUint16(0);
    writeUint16(0);
    writeUint32(headerSize);

    writeUint32(40);
    writeUint32(static_cast<std::uint32_t>(width));
    writeUint32(static_cast<std::uint32_t>(height));
    writeUint16(1);
    writeUint16(24);
    writeUint32(0);
    writeUint32(dataSize);
    writeUint32(0);
    writeUint32(0);
    writeUint32(0);
    writeUint32(0);

    std::vector<unsigned char> rowBuffer(rowSize);

    for (int y = 0; y < height; ++y) {
      const unsigned char *srcRow =
      pixels.data() + static_cast<std::size_t>(y) * size_t(width * 3);

      for (int x = 0; x < width; ++x) {
        unsigned char r = srcRow[3 * x + 0];
        unsigned char g = srcRow[3 * x + 1];
        unsigned char b = srcRow[3 * x + 2];
        rowBuffer[size_t(3 * x + 0)] = b;
        rowBuffer[size_t(3 * x + 1)] = g;
        rowBuffer[size_t(3 * x + 2)] = r;
      }

      for (std::uint32_t p = 0; p < padding; ++p)
        rowBuffer[rowStride + p] = 0;

      out.write(reinterpret_cast<const char *>(rowBuffer.data()),
                static_cast<std::streamsize>(rowSize));
    }

    return static_cast<bool>(out);
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
