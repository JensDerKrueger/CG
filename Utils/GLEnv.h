#pragma once

#include <memory>
#include <string>
#include <functional>

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
  #include <emscripten/html5.h>
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
  #include <GLFW/glfw3.h>
#endif

#include "GLDebug.h"
#include "PerformanceTimer.h"

enum class GLDataType {BYTE, HALF, FLOAT};
enum class GLDepthDataType {DEPTH16, DEPTH24, DEPTH32};
enum class CursorMode {NORMAL, HIDDEN, FIXED};

class GLEnv {
public:
#ifdef __EMSCRIPTEN__
  GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title,
        bool fpsCounter=false, bool sync=true, bool exactPixels=false,
        int major=3, int minor=0, bool core=false);
#else
  GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title,
        bool fpsCounter=false, bool sync=true, bool exactPixels=false,
        int major=4, int minor=1, bool core=false);
#endif

  ~GLEnv();

#ifdef __EMSCRIPTEN__
  void setKeyCallback(em_key_callback_func f, void *userData);
  void setMouseCallbacks(em_mouse_callback_func p,
                         em_mouse_callback_func bu,
                         em_mouse_callback_func bd,
                         em_wheel_callback_func s,
                         em_touch_callback_func ts,
                         em_touch_callback_func te,
                         em_touch_callback_func tm,
                         void *userData);
  void setResizeCallback(em_ui_callback_func f, void *userData);
#else
  void setKeyCallback(GLFWkeyfun f);
  void setKeyCallbacks(GLFWkeyfun f, GLFWcharfun c);
  void setMouseCallbacks(GLFWcursorposfun p, GLFWmousebuttonfun b, GLFWscrollfun s);
  void setResizeCallback(GLFWframebuffersizefun f);
#endif

  void setFpsCallback(std::function<void(double)> fpsCallback) {
    this->fpsCallback = std::move(fpsCallback);
  }

  Dimensions getFramebufferSize() const;
  Dimensions getWindowSize() const;
  bool shouldClose() const;
  void setClose();
  void beginOfFrame();
  void endOfFrame();
  void setSize(int width, int height);

  void setCursorMode(CursorMode mode);

  std::string getGPUString();
  std::string getOpenGlInfoString(bool includeExtensions = false);

  void setFPSAccumulationInterval(uint64_t newAccumulationInterval);
  bool getFPSCounterStatus() const {return fpsCounter;}
  void setFPSCounterStatus(bool fpsCounter);
  double getFps() const {return currentFps;}
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
  double currentFps{0.0};
  bool fpsCounter;
  std::shared_ptr<PerformanceTimer> timer = nullptr;

  uint64_t accumulatedNanoseconds{0};
  uint32_t accumulatedFrames{0};
  double integratedFPS{0.0};
  uint64_t accumulationInterval{1};

  static void errorCallback(int error, const char* description);
  double updateIntegratedFPS(std::uint64_t nanoseconds);

  std::function<void(double)> fpsCallback;

};
