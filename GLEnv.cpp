#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <iostream>

#ifndef SET_ENS_CANVAS
#define ENS_CANVAS "#canvas"
#else
#define ENS_CANVAS SET_ENS_CANVAS
#endif

typedef std::chrono::high_resolution_clock Clock;

#include "GLEnv.h"
#include "GLDebug.h"

#ifdef _WIN32
#ifndef _GLFW_USE_HYBRID_HPG
//choose graphicscard instead of embedded intel-graphics.
extern "C" {
	_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
#endif


void GLEnv::checkGLError(const std::string& id) {
  GLenum e = glGetError();
  if (e != GL_NO_ERROR) {
    std::cerr << "An openGL error occured:" << errorString(e) << " (" << e << ") at " << id << std::endl;
  }
}

void GLEnv::errorCallback(int error, const char* description) {
  // ignore known issue on Apple M1
  if (error == 65544) return;
  std::stringstream s;
  s << description << " (Error number: " << error << ")";
  throw GLException{s.str()};
}

GLEnv::GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title, 
             bool fpsCounter, bool sync, bool exactPixels,
             int major, int minor, bool core) :
#ifndef __EMSCRIPTEN__
  window(nullptr),
#endif
  sync(sync),
  title(title),
  fpsCounter(fpsCounter)
{
#ifdef __EMSCRIPTEN__
  emscripten_set_canvas_element_size(ENS_CANVAS, w, h);

  EmscriptenWebGLContextAttributes attr;
  emscripten_webgl_init_context_attributes(&attr);
  attr.alpha = 1;
  attr.premultipliedAlpha = 0;
  attr.depth = 1;
  attr.stencil = 0;
  attr.antialias = 1;
  attr.majorVersion = major;
  attr.minorVersion = minor;
  attr.enableExtensionsByDefault = true;

  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(ENS_CANVAS, &attr);
  emscripten_webgl_make_context_current(context);

  timer = std::make_shared<CPUTimer>();
#else
  glfwSetErrorCallback(errorCallback);

  if (!glfwInit())
    throw GLException{"GLFW Init Failed"};

  glfwWindowHint(GLFW_SAMPLES, int(s));

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  
  if (core) {
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }


  if (exactPixels) {
    // Disable high-DPI framebuffer scaling on platforms that support it
#ifdef GLFW_SCALE_FRAMEBUFFER          // GLFW 3.4+ generic name
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
#endif

#ifdef __APPLE__
    // For older GLFW versions and explicit macOS control
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif

    // On Windows/X11 this hint has no effect because framebuffer == window size anyway
    // as long as you DON'T enable GLFW_SCALE_TO_MONITOR.
  } else {
#ifdef GLFW_SCALE_FRAMEBUFFER
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
#endif
#ifdef __APPLE__
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif
  }

  // Now create the window; framebuffer will be exactly w×h if exactPixels == true
  window = glfwCreateWindow(int(w), int(h), title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::stringstream s;
    s << "Failed to open GLFW window.";
    glfwTerminate();
    throw GLException{s.str()};
  }

  glfwMakeContextCurrent(window);

  GLenum err{glewInit()};
  if (err != GLEW_OK) {
    std::stringstream s;
    s << "Failed to init GLEW " << glewGetErrorString(err) << std::endl;
    glfwTerminate();
    throw GLException{s.str()};
  }
  setSync(sync);
  timer = std::make_shared<GLTimer>(true);
#endif

}

GLEnv::~GLEnv() {
#ifndef __EMSCRIPTEN__
  glfwDestroyWindow(window);
  glfwTerminate();
#endif
  timer = nullptr;
}

void GLEnv::setFPSAccumulationInterval(uint64_t newAccumulationInterval) {
  accumulationInterval = newAccumulationInterval;
  accumulatedNanoseconds = 0;
  accumulatedFrames = 0;
  integratedFPS = 0;
  currentFps = 0;
}

void GLEnv::setSync(bool sync) {
  this->sync = sync;

#ifdef __EMSCRIPTEN__
  if (sync)
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
  else
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 1);
#else
  if (sync)
    glfwSwapInterval( 1 );
  else
    glfwSwapInterval( 0 );
#endif
}

void GLEnv::setFPSCounterStatus(bool fpsCounter) {
  this->fpsCounter = fpsCounter;
}

void GLEnv::beginOfFrame() {
  const Dimensions dim = getFramebufferSize();
  GL(glViewport(0, 0, GLsizei(dim.width), GLsizei(dim.height)));

  if (fpsCounter && timer) {
    timer->begin();
  }
}

void GLEnv::endOfFrame() {
#ifndef __EMSCRIPTEN__
  glfwSwapBuffers(window);
  glfwPollEvents();
#endif

  if (fpsCounter && timer) {
#ifdef __EMSCRIPTEN__
    // brute-force sync
    uint8_t pixel[4];
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
#endif
    timer->end();
    currentFps = updateIntegratedFPS(timer->elapsedNanoseconds());
    std::stringstream s;
    if (currentFps > 0) {
      s << title << " (" <<static_cast<int>(std::ceil(currentFps)) << " fps)";
    } else {
      s << title << " (measuring fps)";
    }
#ifdef __EMSCRIPTEN__
    emscripten_set_window_title(s.str().c_str());
#else
    glfwSetWindowTitle(window, s.str().c_str());
#endif

    if (fpsCallback) fpsCallback(currentFps);
  }
}

void GLEnv::setSize(int width, int height) {
#ifndef __EMSCRIPTEN__
  glfwSetWindowSize(window, width, height);
#endif
}

double GLEnv::updateIntegratedFPS(std::uint64_t nanoseconds) {
  const std::uint64_t frameNs = accumulationInterval*1000000000ull;
  accumulatedNanoseconds += nanoseconds;
  accumulatedFrames += 1;

  if (accumulatedNanoseconds >= frameNs) {
    integratedFPS = static_cast<double>(accumulatedFrames) *
    1.0e9 /
    static_cast<double>(accumulatedNanoseconds);
    accumulatedNanoseconds = 0;
    accumulatedFrames = 0;
  }

  return integratedFPS;
}


#ifdef __EMSCRIPTEN__
void GLEnv::setKeyCallback(em_key_callback_func f, void *userData) {
  emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
}

void GLEnv::setMouseCallbacks(em_mouse_callback_func p,
                              em_mouse_callback_func bu,
                              em_mouse_callback_func bd,
                              em_wheel_callback_func s,
                              em_touch_callback_func ts,
                              em_touch_callback_func te,
                              em_touch_callback_func tm,
                              void *userData) {
  emscripten_set_mouseup_callback(ENS_CANVAS, userData, EM_TRUE, bu);
  emscripten_set_mousedown_callback(ENS_CANVAS, userData, EM_TRUE, bd);
  emscripten_set_mousemove_callback(ENS_CANVAS, userData, EM_TRUE, p);
  emscripten_set_mouseenter_callback(ENS_CANVAS, userData, EM_TRUE, p);
  emscripten_set_mouseleave_callback(ENS_CANVAS, userData, EM_TRUE, p);

  emscripten_set_touchstart_callback(ENS_CANVAS, userData, EM_TRUE, ts);
  emscripten_set_touchend_callback(ENS_CANVAS,   userData, EM_TRUE, te);
  emscripten_set_touchmove_callback(ENS_CANVAS,  userData, EM_TRUE, tm);
}

void GLEnv::setResizeCallback(em_ui_callback_func f, void *userData) {
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
}

#else
void GLEnv::setKeyCallback(GLFWkeyfun f) {
  glfwSetKeyCallback(window, f);
}

void GLEnv::setKeyCallbacks(GLFWkeyfun f, GLFWcharfun c) {
  glfwSetKeyCallback(window, f);
  glfwSetCharCallback(window, c);
}

void GLEnv::setResizeCallback(GLFWframebuffersizefun f) {
  glfwSetFramebufferSizeCallback(window, f);
}

void GLEnv::setMouseCallbacks(GLFWcursorposfun p, GLFWmousebuttonfun b, GLFWscrollfun s) {
  glfwSetCursorPosCallback(window, p);
  glfwSetMouseButtonCallback(window, b);
  glfwSetScrollCallback(window, s);
}
#endif

Dimensions GLEnv::getFramebufferSize() const {
  int width, height;
#ifdef __EMSCRIPTEN__
  emscripten_get_canvas_element_size(ENS_CANVAS, &width, &height);
#else
  glfwGetFramebufferSize(window, &width, &height);
#endif
  return Dimensions{uint32_t(width), uint32_t(height)};
}

Dimensions GLEnv::getWindowSize() const {
  int width, height;
#ifdef __EMSCRIPTEN__
  emscripten_get_canvas_element_size(ENS_CANVAS, &width, &height);
#else
  glfwGetWindowSize(window, &width, &height);
#endif
  return Dimensions{uint32_t(width), uint32_t(height)};
}

bool GLEnv::shouldClose() const {
#ifdef __EMSCRIPTEN__
  return false;
#else
  return glfwWindowShouldClose(window);
#endif
}

void GLEnv::setClose() {
#ifndef __EMSCRIPTEN__
  glfwSetWindowShouldClose(window, GL_TRUE);
#endif
}

void GLEnv::setTitle(const std::string& title) {
  this->title = title;
}

void GLEnv::setCursorMode(CursorMode mode) {
#ifdef __EMSCRIPTEN__
  switch (mode) {
    case CursorMode::NORMAL :
//      emscripten_set_element_css_property(ENS_CANVAS, "cursor", "pointer");
      emscripten_request_pointerlock(ENS_CANVAS, EM_FALSE);
      break;
    case CursorMode::HIDDEN :
  //    emscripten_set_element_css_property(ENS_CANVAS, "cursor", "none");
      emscripten_hide_mouse();
      emscripten_request_pointerlock(ENS_CANVAS, EM_FALSE);
      break;
    case CursorMode::FIXED :
   //   emscripten_set_element_css_property(ENS_CANVAS, "cursor", "none");
      emscripten_request_pointerlock(ENS_CANVAS, EM_TRUE);
      emscripten_hide_mouse();
      break;
  }
#else
  switch (mode) {
    case CursorMode::NORMAL :
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      break;
    case CursorMode::HIDDEN :
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      break;
    case CursorMode::FIXED :
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      break;
  }
#endif
}

std::string GLEnv::getGPUString() {
  auto ptrToString = [](const GLubyte* s) -> std::string {
    return s ? reinterpret_cast<const char*>(s) : std::string("<null>");
  };
  return ptrToString(glGetString(GL_RENDERER));
}

std::string GLEnv::getOpenGlInfoString(bool includeExtensions) {
  auto ptrToString = [](const GLubyte* s) -> std::string {
    return s ? reinterpret_cast<const char*>(s) : std::string("<null>");
  };

  // glGetString requires a current context; if not, calls typically return NULL.
  std::string vendor = ptrToString(glGetString(GL_VENDOR));
  std::string renderer = ptrToString(glGetString(GL_RENDERER));
  std::string version = ptrToString(glGetString(GL_VERSION));
  std::string glslVersion = ptrToString(glGetString(GL_SHADING_LANGUAGE_VERSION));

  GLint majorVersion = 0;
  GLint minorVersion = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
  glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

#ifdef GL_CONTEXT_PROFILE_MASK
  GLint profileMask = 0;
  glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
#endif

#ifdef GL_CONTEXT_FLAGS
  GLint contextFlags = 0;
  glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
#endif

#ifdef GL_CONTEXT_PROFILE_MASK
  auto profileMaskToString = [&](GLint mask) -> std::string {
    if (mask & GL_CONTEXT_CORE_PROFILE_BIT) return "Core";
    if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) return "Compatibility";
    return "Unknown";
  };
#endif

  std::ostringstream out;
  out << "OpenGL Info\n"
  << "-----------\n"
  << "Vendor:    " << vendor << "\n"
  << "Renderer:  " << renderer << "\n"
  << "Version:   " << version << "\n"
  << "GLSL:      " << glslVersion << "\n"
  << "Major/Minor (queried): " << majorVersion << "." << minorVersion << "\n";

#ifdef GL_CONTEXT_PROFILE_MASK
  out << "Profile:   " << profileMaskToString(profileMask) << "\n";
#endif

#ifdef GL_CONTEXT_FLAGS
  out << "Context Flags:";
  bool anyFlag = false;
#ifdef GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT
  if (contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) { out << " ForwardCompatible"; anyFlag = true; }
#endif
#ifdef GL_CONTEXT_FLAG_DEBUG_BIT
  if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) { out << " Debug"; anyFlag = true; }
#endif
#ifdef GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
  if (contextFlags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT) { out << " RobustAccess"; anyFlag = true; }
#endif
#ifdef GL_CONTEXT_FLAG_NO_ERROR_BIT
  if (contextFlags & GL_CONTEXT_FLAG_NO_ERROR_BIT) { out << " NoError"; anyFlag = true; }
#endif
  if (!anyFlag) out << " <none/unknown>";
  out << "\n";
#endif

  if (includeExtensions) {
    out << "\nExtensions\n----------\n";

    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    if (extensionCount > 0) {
#ifdef GL_VERSION_3_0
      // Modern way (GL 3.0+): glGetStringi
      for (GLint i = 0; i < extensionCount; ++i) {
        out << ptrToString(glGetStringi(GL_EXTENSIONS, static_cast<GLuint>(i))) << "\n";
      }
#else
      out << "<glGetStringi not available in this build>\n";
#endif
    } else {
      // Fallback for older contexts: GL_EXTENSIONS as a single space-separated string
      out << ptrToString(glGetString(GL_EXTENSIONS)) << "\n";
    }
  }

  return out.str();
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
