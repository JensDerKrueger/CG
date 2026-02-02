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
