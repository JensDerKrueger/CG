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

  void setBackground(float r, float g, float b, float a) { setBackground(Vec4(r,g,b,a));}
  void setBackground(const Vec4& color) { background = color;}

protected:
  GLEnv glEnv;
  CommandInterpreter interpreter;
  std::function<void(const std::string&)> messageCallback;

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


  void setAnimation(bool animationActive);
  bool getAnimation() const;
  void resetAnimation();
  float getAspect() const;
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
  Vec4 getBackground() const {return background;}

  void setCallbacks(std::function<void(double)> fpsCallback,
                    std::function<void(const std::string&)> messageCallback);

  void closeWindow();

private:
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
  bool animationActive;
  TrisDrawType lastTrisType;
  GLsizei lastTrisCount;
  bool lastLighting;
  double startTime;

  std::string logDir{""};
  bool interaction{true};
  Vec4 background{0,0,0,1};

  std::string scriptLogFile{"script.txt"};
  bool scriptRunning{false};


  void shaderUpdate();
  void mainLoop();

#ifdef __EMSCRIPTEN__
  float xMousePos;
  float yMousePos;

  static void mainLoopWrapper(void* arg);
  static bool sizeCallback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) ;
  static int map_modifiers_to_bitfield(const EmscriptenKeyboardEvent* e);
  static int map_key_string_to_code(const char* code);
  static bool keyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
  static bool cursorPositionCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
  static bool mouseButtonUpCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
  static bool touchStartCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
  static bool touchMoveCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
  static bool touchEndCallback(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
  static bool mouseButtonDownCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
  static bool scrollCallback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData);
#else
  static GLApp* staticAppPtr;
  static void sizeCallback(GLFWwindow* window, int width, int height);
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void keyCharCallback(GLFWwindow* window, unsigned int codepoint);
  static void cursorPositionCallback(GLFWwindow* window, double xPosition, double yPosition);
  static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mods);
  static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);
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
