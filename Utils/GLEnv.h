#pragma once

#include <memory>
#include <string>
#include <chrono>

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
  #include <emscripten/html5.h>
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
#endif

#include "GLDebug.h"

enum class GLDataType {BYTE, HALF, FLOAT};
enum class GLDepthDataType {DEPTH16, DEPTH24, DEPTH32};
enum class CursorMode {NORMAL, HIDDEN, FIXED};

class GLEnv {
public:
  GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title, bool fpsCounter=false, bool sync=true, int major=2, int minor=1, bool core=false);
  ~GLEnv();

#ifdef __EMSCRIPTEN__
  void setKeyCallback(em_key_callback_func f, void *userData);
  void setMouseCallbacks(em_mouse_callback_func p,
                         em_mouse_callback_func b,
                         em_mouse_callback_func bu,
                         em_mouse_callback_func bd,
                         em_wheel_callback_func s,
                         void *userData);
  void setResizeCallback(em_ui_callback_func f, void *userData);
#else
  void setKeyCallback(GLFWkeyfun f);
  void setKeyCallbacks(GLFWkeyfun f, GLFWcharfun c);
  void setMouseCallbacks(GLFWcursorposfun p, GLFWmousebuttonfun b, GLFWscrollfun s);
  void setResizeCallback(GLFWframebuffersizefun f);
#endif


  Dimensions getFramebufferSize() const;
  Dimensions getWindowSize() const;
  bool shouldClose() const;
  void setClose();
  void endOfFrame();
  
  void setCursorMode(CursorMode mode);
  
  void setFPSCounter(bool fpsCounter);
  void setSync(bool sync);
  bool getSync() const {return sync;}

  static void checkGLError(const std::string& id);

  void setTitle(const std::string& title);

private:
#ifndef __EMSCRIPTEN__
  GLFWwindow* window;
#endif
  bool sync;
  std::string title;
  bool fpsCounter;
  std::chrono::high_resolution_clock::time_point last;
  uint64_t frameCount;
		
  static void errorCallback(int error, const char* description);
};
