#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <iostream>

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
             bool fpsCounter, bool sync, int major, int minor, bool core) :
#ifndef __EMSCRIPTEN__
  window(nullptr),
#endif
  sync(sync),
  title(title),
  fpsCounter(fpsCounter),
  last(Clock::now())
{
#ifdef __EMSCRIPTEN__
  emscripten_set_canvas_element_size("#canvas", w, h);

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

  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context("#canvas", &attr);
  emscripten_webgl_make_context_current(context);

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
#endif

}

GLEnv::~GLEnv() {
#ifndef __EMSCRIPTEN__
  glfwDestroyWindow(window);
  glfwTerminate();
#endif
}

void GLEnv::setSync(bool sync) {
  this->sync = sync;

#ifdef __EMSCRIPTEN__
  // TODO: check this
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


void GLEnv::setFPSCounter(bool fpsCounter) {
  this->fpsCounter = fpsCounter;
}

void GLEnv::endOfFrame() {
#ifndef __EMSCRIPTEN__
  glfwSwapBuffers(window);
  glfwPollEvents();
#endif

  if (fpsCounter) {
    frameCount++;
    auto now = Clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
    if(diff.count() > 1e6) {
        auto fps = static_cast<double>(frameCount)/diff.count()*1.e6;
      std::stringstream s;
      s << title << " (" << static_cast<int>(std::ceil(fps)) << " fps)";
#ifdef __EMSCRIPTEN__
      emscripten_set_window_title(s.str().c_str());
#else
      glfwSetWindowTitle(window, s.str().c_str());
#endif
      frameCount = 0;
      last = now;
    }
  }
}

#ifdef __EMSCRIPTEN__
void GLEnv::setKeyCallback(em_key_callback_func f, void *userData) {
  emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, EM_TRUE, f);
}

void GLEnv::setMouseCallbacks(em_mouse_callback_func p,
                              em_mouse_callback_func b,
                              em_wheel_callback_func s,
                              void *userData) {
  emscripten_set_click_callback("#canvas", userData, EM_TRUE, b);
  emscripten_set_mousedown_callback("#canvas", userData, EM_TRUE, b);
  emscripten_set_mouseup_callback("#canvas", userData, EM_TRUE, b);
  emscripten_set_dblclick_callback("#canvas", userData, EM_TRUE, b);
  emscripten_set_mousemove_callback("#canvas", userData, EM_TRUE, p);
  emscripten_set_mouseenter_callback("#canvas", userData, EM_TRUE, p);
  emscripten_set_mouseleave_callback("#canvas", userData, EM_TRUE, p);
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
  emscripten_get_canvas_element_size("#canvas", &width, &height);
#else
  glfwGetFramebufferSize(window, &width, &height);
#endif
  return Dimensions{uint32_t(width), uint32_t(height)};
}


Dimensions GLEnv::getWindowSize() const {
  int width, height;
#ifdef __EMSCRIPTEN__
  emscripten_get_canvas_element_size("#canvas", &width, &height);
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
//      emscripten_set_element_css_property("#canvas", "cursor", "pointer");
      emscripten_request_pointerlock("#canvas", EM_FALSE);
      break;
    case CursorMode::HIDDEN :
  //    emscripten_set_element_css_property("#canvas", "cursor", "none");
      emscripten_hide_mouse();
      emscripten_request_pointerlock("#canvas", EM_FALSE);
      break;
    case CursorMode::FIXED :
   //   emscripten_set_element_css_property("#canvas", "cursor", "none");
      emscripten_request_pointerlock("#canvas", EM_TRUE);
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

