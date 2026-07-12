#include <GLApp.h>
#include <vector>

class MyGLApp : public GLApp {
public:
  constexpr static float sqrt3{1.7320508076f};

  const std::vector<float> upperTriangle{
     0.0f,  2.0f * sqrt3 / 3.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f
  };

  const std::vector<float> rightTriangle{
     0.0f,  2.0f * sqrt3 / 3.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
     1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f
  };

  const std::vector<float> leftTriangle{
     0.0f,  2.0f * sqrt3 / 3.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f * sqrt3 / 3.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f
  };

  float angle{0.0f};

  MyGLApp()
    : GLApp(800, 600, 4, "Assignment 12 - Transformations")
  {}

  virtual void init() override {
    setBackground(0.04f, 0.045f, 0.055f, 1.0f);
  }

  virtual void animate(double animationTime) override {
    (void)animationTime;
  }

  virtual void draw() override {
    setDrawTransform(Mat4{});
    drawTriangles(upperTriangle, TrisDrawType::LIST, false, false);

    setDrawTransform(Mat4{});
    drawTriangles(rightTriangle, TrisDrawType::LIST, false, false);

    setDrawTransform(Mat4{});
    drawTriangles(leftTriangle, TrisDrawType::LIST, false, false);
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);

    const float ratio = fbDim.aspect();
    setDrawProjection(Mat4::ortho(-ratio * sqrt3, ratio * sqrt3,
                                  -sqrt3, sqrt3,
                                  -10.0f, 10.0f));
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (key == GLENV_KEY_ESCAPE && action == GLENV_PRESS)
      closeWindow();
  }
} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
    std::vector<std::string> args{ argv + 1, argv + argc };
#endif
    try {
        myApp.run();
    }
    catch (const GLException& e) {
        std::stringstream ss;
        ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
        std::cerr << ss.str().c_str() << std::endl;
#else
        MessageBoxA(
            NULL,
            ss.str().c_str(),
            "OpenGL Error",
            MB_ICONERROR | MB_OK
        );
#endif
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
