#pragma once

#include <fstream>
#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include "Vec2.h"
#include "Image.h"

namespace BMP {
  class BMPException : public std::exception {
    public:
      BMPException(const std::string& whatStr) : whatStr(whatStr) {}
      virtual const char* what() const throw() {
        return whatStr.c_str();
      }
    private:
      std::string whatStr;
  };

  bool save(const std::string& filename, const Image& source, bool ignoreSize=false);

  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<uint8_t>& data, uint8_t iComponentCount = 3,
            bool ignoreSize=false);

  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<float>& data, uint8_t iComponentCount = 3,
            bool ignoreSize=false);


  Image load(const std::string& filename);

  void blit(const Image& source, const Vec2ui& sourceStart, const Vec2ui& sourceEnd,
            Image& target, const Vec2ui& targetStart, bool skipChecks=false);
}

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
