#pragma once

#include <iostream>
#include <exception>

class GLException : public std::exception {
public:
  GLException(const std::string& whatStr) : whatStr(whatStr) {}
  virtual const char* what() const throw() {
    return whatStr.c_str();
  }
private:
  std::string whatStr;
};

struct Dimensions {
  uint32_t width;
  uint32_t height;
  
  float aspect() const {return float(width)/float(height);}
};


std::string errorString(GLenum glerr);

#ifndef NDEBUG

// under some circumstances the glError loops below do not
// terminate, either glError itself causes an erro or does
// not reset the error state. Neither should happen, but
// still do
#define MAX_GL_ERROR_COUNT 10

void errorOut(const std::string& statement, const std::string& location,
              uint32_t line, const std::string& file, uint32_t errnum);

# define GL(stmt)                                                      \
  do {                                                                 \
    GLenum glerr;                                                      \
    uint32_t counter = 0;                                              \
    while((glerr = glGetError()) != GL_NO_ERROR) {                     \
      errorOut(#stmt, "before", __LINE__, __FILE__,                    \
              static_cast<uint32_t>(glerr));                           \
      counter++;                                                       \
      if (counter > MAX_GL_ERROR_COUNT) break;                         \
    }                                                                  \
    stmt;                                                              \
    counter = 0;                                                       \
    while((glerr = glGetError()) != GL_NO_ERROR) {                     \
      errorOut(#stmt, "in", __LINE__, __FILE__,                        \
              static_cast<uint32_t>(glerr));                           \
      counter++;                                                       \
      if (counter > MAX_GL_ERROR_COUNT) break;                         \
    }                                                                  \
  } while(0)
#else
# define GL(stmt) do { stmt; } while(0)
#endif

void checkAndThrow();
void checkAndThrowShader(GLuint shader);
void checkAndThrowProgram(GLuint program);

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
