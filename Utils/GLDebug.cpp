#include <sstream>
#include <vector>

#include "GLEnv.h"
#include "GLDebug.h"

std::string errorString(GLenum glerr) {
  switch (glerr) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    default: return "Unknown Error";
  }
}

void errorOut(const std::string& statement, const std::string& location,
              uint32_t line, const std::string& file, uint32_t errnum) {
  std::cerr << "GL error calling " << statement << " " << location <<  " " << line
            << " (" << file << "):" <<  errorString(errnum) << " (" << errnum << ")" << std::endl;
}

void checkAndThrow() {
  GLenum e = glGetError();
  if (e != GL_NO_ERROR) {
    std::stringstream s;
    s << "An openGL error occured:" << errorString(e);
    throw GLException{s.str()};
  }
}

void checkAndThrowShader(GLuint shader) {
  GLint success[1] = { GL_TRUE };
  glGetShaderiv(shader, GL_COMPILE_STATUS, success);
  if(success[0] == GL_FALSE) {
    GLint log_length{0};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    log_length = std::min(static_cast<GLint>(4096), log_length);
    std::vector<GLchar> log((size_t)log_length);
    glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), NULL, log.data());
    std::string str{log.data()};
#ifdef __EMSCRIPTEN__
    std::cout << str << std::endl;
#endif
    throw GLException{str};
  }
}

void checkAndThrowProgram(GLuint program) {
  GLint linked{GL_TRUE};
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if(linked != GL_TRUE) {
    GLint log_length{0};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    log_length = std::min(static_cast<GLint>(4096), log_length);
    std::vector<GLchar> log((size_t)log_length);
    glGetProgramInfoLog(program, static_cast<GLsizei>(log.size()), NULL, log.data());
    std::string str{log.data()};
    throw GLException{str};
  }
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
