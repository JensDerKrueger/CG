#pragma once

#include <string>

#include "GLEnv.h"
#include "GLProgram.h"
#include "GLArray.h"
#include "GLBuffer.h"
#include "GLTexture2D.h"
#include "Image.h"
#include "GLAppKeyTranslation.h"

#include "CommandInterpreter.h"
#include "GLScreenshot.h"

#ifdef _WIN32
std::vector<std::string> getArgsWindows();
#endif

enum class LineDrawType {
  LIST,
  STRIP,
  LOOP
};

enum class TrisDrawType {
  LIST,
  STRIP,
  FAN
};

class GLApp {
public:
  GLApp(uint32_t w=640, uint32_t h=480, uint32_t s=4,
        const std::string& title = "My OpenGL App",
        bool fpsCounter=true, bool sync=true, bool exactPixels=false,
        const std::vector<std::string>& args = {});
  virtual ~GLApp();
  void run();
  void setAnimation(bool animationActive) {
    if (this->animationActive && !animationActive) {
#ifdef __EMSCRIPTEN__
      resumeTime = emscripten_performance_now()/1000.0;
#else
      resumeTime = glfwGetTime();
#endif
    }

    if (!this->animationActive && animationActive) {
      if (resumeTime == 0) {
#ifdef __EMSCRIPTEN__
        startTime = emscripten_performance_now()/1000.0;
#else
        startTime = glfwGetTime();
#endif
      } else {
#ifdef __EMSCRIPTEN__
        startTime += emscripten_performance_now()/1000.0-resumeTime;
#else
        startTime += glfwGetTime()-resumeTime;
#endif
      }
    }

    this->animationActive = animationActive;
  }
  bool getAnimation() const {
    return animationActive;
  }
  void resetAnimation() {
#ifdef __EMSCRIPTEN__
    startTime = emscripten_performance_now()/1000.0;
#else
    startTime = glfwGetTime();
#endif
    resumeTime = 0;
    animate(0);
  }

  float getAspect() const {
    const Dimensions d = glEnv.getWindowSize();
    return float(d.width)/float(d.height);
  }
  
  void setImageFilter(GLint magFilter, GLint minFilter);

  void drawRect(const Vec4& color, const Vec2& bl, const Vec2& tr);
  void drawRect(const Vec4& color,
                const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                const Vec3& tr=Vec3{1.0f,1.0f,0.0f});

  void drawImage(const GLTexture2D& image, const Vec2& bl, const Vec2& tr);
  void drawImage(const Image& image, const Vec2& bl, const Vec2& tr);
  void drawImage(const GLTexture2D& image,
                 const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                 const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                 const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                 const Vec3& tr=Vec3{1.0f,1.0f,0.0f});
  void drawImage(const Image& image,
                 const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                 const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                 const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                 const Vec3& tr=Vec3{1.0f,1.0f,0.0f});
  void drawTriangles(const std::vector<float>& data, TrisDrawType t, bool wireframe, bool lighting);
  void redrawTriangles(bool wireframe);

  Mat4 computeImageTransform(const Vec2ui& imageSize) const;
  Mat4 computeImageTransformFixedHeight(const Vec2ui& imageSize,
                                        float height=1.0f,
                                        const Vec3& center=Vec3{0.0f,0.0f,0.0f}) const;
  Mat4 computeImageTransformFixedWidth(const Vec2ui& imageSize,
                                       float width=1.0f,
                                       const Vec3& center=Vec3{0.0f,0.0f,0.0f}) const;

  void drawLines(const std::vector<float>& data, LineDrawType t, float lineThickness=1.0f);
  void drawPoints(const std::vector<float>& data, float pointSize=1.0f, bool useTex=false);
  void setDrawProjection(const Mat4& mat);
  void setDrawTransform(const Mat4& mat);
  
  Mat4 getDrawProjection() const;
  Mat4 getDrawTransform() const;
  
  void resetPointTexture(uint32_t resolution=64);
  void setPointTexture(const std::vector<uint8_t>& shape, uint32_t x,
                       uint32_t y, uint8_t components);
  void setPointTexture(const Image& shape);
  void setPointHighlightTexture(const Image& shape);
  void resetPointHighlightTexture();
  void setBackground(float r, float g, float b, float a) { setBackground(Vec4(r,g,b,a));}
  void setBackground(const Vec4& color) { background = color;}

  virtual void reset() {}
  virtual void init() {}
  virtual void draw() {}
  virtual void animate(double animationTime) {}
  
  virtual void resize(const Dimensions winDim, const Dimensions fbDim);
  virtual void keyboard(int key, int scancode, int action, int mods) {}
  virtual void keyboardChar(unsigned int key) {}
  virtual void mouseMove(double xPosition, double yPosition) {}
  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) {}
  virtual void mouseWheel(double x_offset, double y_offset, double xPosition, double yPosition) {}

  void setCallbacks(std::function<void(double)> fpsCallback,
                    std::function<void(const std::string&)> messageCallback) {
    glEnv.setFpsCallback(fpsCallback);
    this->messageCallback = messageCallback;
  }

protected:
  GLEnv glEnv;
  Mat4 p;
  Mat4 mv;
  Mat4 mvi;
  GLProgram simpleProg;
  GLProgram simplePointProg;
  GLProgram simpleSpriteProg;
  GLProgram simpleHLSpriteProg;
  GLProgram simpleTexProg;
  GLProgram simpleLightProg;
  GLArray simpleArray;
  GLBuffer simpleVb;
  GLTexture2D raster;
  GLTexture2D pointSprite;
  GLTexture2D pointSpriteHighlight;
  double resumeTime;
#ifdef __EMSCRIPTEN__
  float xMousePos;
  float yMousePos;
#endif
  std::string logDir{""};
  bool interaction{true};
  Vec4 background{0,0,0,1};

  void shaderUpdate();

  void closeWindow() {
    glEnv.setClose();
  }

  std::string scriptLogFile{"script.txt"};
  bool scriptRunning{false};
  CommandInterpreter interpreter;
  std::function<void(const std::string&)> messageCallback;

private:
  bool animationActive;
  TrisDrawType lastTrisType;
  GLsizei lastTrisCount;
  bool lastLighting;
  double startTime;

  void mainLoop();

#ifdef __EMSCRIPTEN__
  static void mainLoopWrapper(void* arg) {
    GLApp* app = static_cast<GLApp*>(arg);
    app->mainLoop();
  }

  static bool sizeCallback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    //TODO
    return EM_TRUE;
  }

  static int map_modifiers_to_bitfield(const EmscriptenKeyboardEvent* e) {
    int mods = 0;
    if (e->shiftKey) mods |= (1 << 0);   // Shift → Bit 0
    if (e->ctrlKey)  mods |= (1 << 1);   // Ctrl → Bit 1
    if (e->altKey)   mods |= (1 << 2);   // Alt  → Bit 2
    return mods;
  }

  static int map_key_string_to_code(const char* code) {
    // Alphanumeric (A-Z)
    if (strncmp(code, "Key", 3) == 0 && strlen(code) == 4) {
      char c = code[3];
      if (c >= 'A' && c <= 'Z') {
        return GLENV_KEY_A + (c - 'A');
      }
    }

    // Number row (0–9)
    if (strncmp(code, "Digit", 5) == 0 && strlen(code) == 6) {
      char d = code[5];
      if (d >= '0' && d <= '9') {
        return GLENV_KEY_0 + (d - '0');
      }
    }

    // Numpad
    if (strncmp(code, "Numpad", 6) == 0) {
      const char* sub = code + 6;
      if (strcmp(sub, "0") == 0) return GLENV_KEY_KP_0;
      if (strcmp(sub, "1") == 0) return GLENV_KEY_KP_1;
      if (strcmp(sub, "2") == 0) return GLENV_KEY_KP_2;
      if (strcmp(sub, "3") == 0) return GLENV_KEY_KP_3;
      if (strcmp(sub, "4") == 0) return GLENV_KEY_KP_4;
      if (strcmp(sub, "5") == 0) return GLENV_KEY_KP_5;
      if (strcmp(sub, "6") == 0) return GLENV_KEY_KP_6;
      if (strcmp(sub, "7") == 0) return GLENV_KEY_KP_7;
      if (strcmp(sub, "8") == 0) return GLENV_KEY_KP_8;
      if (strcmp(sub, "9") == 0) return GLENV_KEY_KP_9;
      if (strcmp(sub, "Decimal") == 0) return GLENV_KEY_KP_DECIMAL;
      if (strcmp(sub, "Divide") == 0) return GLENV_KEY_KP_DIVIDE;
      if (strcmp(sub, "Multiply") == 0) return GLENV_KEY_KP_MULTIPLY;
      if (strcmp(sub, "Subtract") == 0) return GLENV_KEY_KP_SUBTRACT;
      if (strcmp(sub, "Add") == 0) return GLENV_KEY_KP_ADD;
      if (strcmp(sub, "Enter") == 0) return GLENV_KEY_KP_ENTER;
    }

    // Function keys
    if (strncmp(code, "F", 1) == 0 && strlen(code) <= 3) {
      int fn = atoi(code + 1);
      if (fn >= 1 && fn <= 12) return GLENV_KEY_F1 + (fn - 1);
    }

    // Other named keys
    if (strcmp(code, "Enter") == 0) return GLENV_KEY_ENTER;
    if (strcmp(code, "Space") == 0) return GLENV_KEY_SPACE;
    if (strcmp(code, "Tab") == 0) return GLENV_KEY_TAB;
    if (strcmp(code, "Backspace") == 0) return GLENV_KEY_BACKSPACE;
    if (strcmp(code, "Escape") == 0) return GLENV_KEY_ESCAPE;

    if (strcmp(code, "ArrowUp") == 0) return GLENV_KEY_UP;
    if (strcmp(code, "ArrowDown") == 0) return GLENV_KEY_DOWN;
    if (strcmp(code, "ArrowLeft") == 0) return GLENV_KEY_LEFT;
    if (strcmp(code, "ArrowRight") == 0) return GLENV_KEY_RIGHT;


    // Default: unknown or unsupported
    return 0;
  }

  static bool keyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {

    // TODO: handle modifiers properly
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    const int keyCode = map_key_string_to_code(keyEvent->code);
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
      glApp->keyboardChar(keyCode);
    glApp->keyboard(keyCode, keyCode, eventType, map_modifiers_to_bitfield(keyEvent));

    return EM_TRUE;
  }

  static bool cursorPositionCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->xMousePos = mouseEvent->targetX;
    glApp->yMousePos = mouseEvent->targetY;

    glApp->mouseMove(glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }

  static bool mouseButtonUpCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    glApp->mouseButton(mouseEvent->button,
                       GLENV_MOUSE_RELEASE,
                       0,
                       glApp->xMousePos,
                       glApp->yMousePos);
    return EM_TRUE;
  }

  static bool touchStartCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    glApp->mouseButton(GLENV_MOUSE_BUTTON_LEFT,
                       GLENV_MOUSE_PRESS,
                       0,
                       glApp->xMousePos,
                       glApp->yMousePos);
    return EM_TRUE;
  }

  static bool touchMoveCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->xMousePos = touchEvent->touches[0].clientX;
    glApp->yMousePos = touchEvent->touches[0].clientY;

    glApp->mouseMove(glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }

  static bool touchEndCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    glApp->mouseButton(GLENV_MOUSE_BUTTON_LEFT,
                       GLENV_MOUSE_RELEASE,
                       0,
                       glApp->xMousePos,
                       glApp->yMousePos);
    return EM_TRUE;
  }

  static bool mouseButtonDownCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    glApp->mouseButton(mouseEvent->button,
                       GLENV_MOUSE_PRESS,
                       0,
                       glApp->xMousePos,
                       glApp->yMousePos);
    return EM_TRUE;
  }

  static bool scrollCallback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    // TODO

    return EM_TRUE;
  }
#else
  static GLApp* staticAppPtr;
  static void sizeCallback(GLFWwindow* window, int width, int height) {
    if (staticAppPtr) {
      const Dimensions winDim = staticAppPtr->glEnv.getWindowSize();
      const Dimensions fbDim  = staticAppPtr->glEnv.getFramebufferSize();
      staticAppPtr->resize(winDim, fbDim);
    }
  }
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (staticAppPtr) staticAppPtr->keyboard(key, scancode, action, mods);
  }
  static void keyCharCallback(GLFWwindow* window, unsigned int codepoint) {
    if (staticAppPtr) staticAppPtr->keyboardChar(codepoint);
  }
  static void cursorPositionCallback(GLFWwindow* window, double xPosition, double yPosition) {
    if (staticAppPtr) staticAppPtr->mouseMove(xPosition, yPosition);
  }
  static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mods) {
    if (staticAppPtr) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      staticAppPtr->mouseButton(button, state, mods, xpos, ypos);
    }
  }
  static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    if (staticAppPtr) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      staticAppPtr->mouseWheel(x_offset, y_offset, xpos, ypos);
    }
  }
#endif

  void setInteractionCallbacks();

  void triangulate(const Vec3& p0,
                   const Vec3& p1, const Vec4& c1,
                   const Vec3& p2, const Vec4& c2,
                   const Vec3& p3,
                   float lineThickness,
                   std::vector<float>& trisData);

  void initScript(const std::vector<std::string>& args);
  bool writeScriptLog(const std::string& text) const;
  void processScript();
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
