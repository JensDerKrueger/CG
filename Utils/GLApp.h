#pragma once

#include <string>

#include "GLEnv.h"
#include "GLProgram.h"
#include "GLArray.h"
#include "GLBuffer.h"
#include "GLTexture2D.h"
#include "Image.h"
#include "GLAppKeyTranslation.h"

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
        bool fpsCounter=true, bool sync=true);
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

  virtual void init() {}
  virtual void draw() {}
  virtual void animate(double animationTime) {}
  
  virtual void resize(int width, int height);
  virtual void keyboard(int key, int scancode, int action, int mods) {}
  virtual void keyboardChar(unsigned int key) {}
  virtual void mouseMove(double xPosition, double yPosition) {}
  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) {}
  virtual void mouseWheel(double x_offset, double y_offset, double xPosition, double yPosition) {}
    
protected:
  GLEnv glEnv;
  Mat4 p;
  Mat4 mv;
  Mat4 mvi;
  GLProgram simpleProg;
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

  void shaderUpdate();

  void closeWindow() {
    glEnv.setClose();
  }
  
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
  static bool keyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {

    // TODO: handle modifiers properly
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
      glApp->keyboardChar(keyEvent->key[0]);
    glApp->keyboard(keyEvent->key[0], keyEvent->key[0], eventType, 0);

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
  static bool mouseButtonCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->mouseButton(mouseEvent->button, 0, 0, glApp->xMousePos, glApp->yMousePos);
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
    if (staticAppPtr) staticAppPtr->resize(width, height);
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

  void triangulate(const Vec3& p0,
                   const Vec3& p1, const Vec4& c1,
                   const Vec3& p2, const Vec4& c2,
                   const Vec3& p3,
                   float lineThickness,
                   std::vector<float>& trisData);

};
