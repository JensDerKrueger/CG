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
