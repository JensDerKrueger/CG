#pragma once

#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "Vec3.h"

class OBJFile {
public:
  OBJFile(const std::string& filename, bool normalize=false);
  
  typedef std::array<size_t, 3> IndexType;

  std::vector<IndexType> indices;
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  
private:
  void ltrim(std::string &s);
  void rtrim(std::string &s);
  void trim(std::string &s);
  std::vector<std::string> tokenize(const std::string& str, size_t startpos);

  template <typename T> T fromStr(const std::string& str) {
    T t;
    std::istringstream iss{str};
    iss >> std::dec >> t;
    return t;
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
