#include <FontRenderer.h>
#include <GLApp.h>
#include <Quaternion.h>
#include <Tessellation.h>

#include <cmath>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

enum class RotationMode {
  Euler = 0,
  Quaternion = 1
};

class MyGLApp : public GLApp {
public:
  FontRenderer fontRenderer{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fontEngine{nullptr};
  std::vector<float> cube{makeCubeData()};
  Vec3 rotationAngles{0.0f, 0.0f, 0.0f};
  Quaternion quaternionRotation{0.0f, 0.0f, 0.0f, 1.0f};
  RotationMode rotationMode{RotationMode::Euler};

  MyGLApp()
    : GLApp(800, 600, 4, "Exercise 13 - Euler and Quaternion Rotations")
  {}

  static std::vector<float> makeCubeData() {
    const Tessellation cubeMesh =
      Tessellation::genBrick({0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}).unpack();
    const std::vector<float>& vertices = cubeMesh.getVertices();
    const std::vector<Vec4> faceColors{
      {0.95f, 0.18f, 0.18f, 1.0f},
      {0.15f, 0.55f, 0.95f, 1.0f},
      {0.18f, 0.72f, 0.28f, 1.0f},
      {0.96f, 0.72f, 0.12f, 1.0f},
      {0.58f, 0.30f, 0.92f, 1.0f},
      {0.00f, 0.68f, 0.70f, 1.0f}
    };

    std::vector<float> data;
    data.reserve((vertices.size() / 3) * 7);

    for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
      const Vec4& color = faceColors[(i / 18) % faceColors.size()];
      data.insert(data.end(), {
        vertices[i + 0], vertices[i + 1], vertices[i + 2],
        color.r, color.g, color.b, color.a
      });
    }

    return data;
  }

  Quaternion axisAngleQuaternion(const Vec3& axis, const float degrees) const {
    // The sign matches the rotation convention used by Mat4::rotationX/Y/Z.
    const float halfAngle = -degrees * Tessellation::PI / 360.0f;
    return {axis * std::sin(halfAngle), std::cos(halfAngle)};
  }

  Mat4 computeEulerRotation() const {
    // TODO Task 1:
    // Build the cube rotation from Euler angles. Use rotationAngles.x,
    // rotationAngles.y and rotationAngles.z as degrees around the x-, y- and
    // z-axis. Choose and document one multiplication order, for example:
    //
    //   R = Rz * Ry * Rx
    //
    // Remember: in this framework matrix * vector is used, so the rightmost
    // matrix is applied first.
    return Mat4{};
  }

  void rotateQuaternion(const Vec3& axis, const float degrees) {
    // TODO Task 2:
    // Create a small delta rotation with axisAngleQuaternion(...) and compose
    // it with quaternionRotation. Use delta * quaternionRotation if the key
    // should rotate around the fixed coordinate axes.
  }

  void resetRotation() {
    rotationAngles = {0.0f, 0.0f, 0.0f};
    quaternionRotation = {0.0f, 0.0f, 0.0f, 1.0f};
  }

  void drawStatusText() {
    if (!fontEngine) return;

    const std::string modeText =
      rotationMode == RotationMode::Euler ? "Mode: Eulerwinkel" : "Mode: Quaternionen";
    fontEngine->render(modeText, getAspect(), 0.035f, {0.0f, -0.90f},
                       Alignment::Center, {1.0f, 1.0f, 1.0f, 0.95f});
  }

  virtual void init() override {
    setBackground(0.04f, 0.045f, 0.055f, 1.0f);
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDisable(GL_CULL_FACE));
    fontEngine = fontRenderer.generateFontEngine();
  }

  virtual void draw() override {
    setDrawTransform(rotationMode == RotationMode::Euler
      ? computeEulerRotation()
      : quaternionRotation.computeRotation());
    drawTriangles(cube, TrisDrawType::LIST, false, false);

    drawStatusText();
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);

    const Mat4 projection = Mat4::perspective(45.0f, fbDim.aspect(), 0.1f, 100.0f);
    const Mat4 view = Mat4::lookAt({0.0f, 0.0f, 5.0f},
                                   {0.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f});
    setDrawProjection(projection * view);
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    (void)scancode;
    (void)mods;
    if (action == GLENV_RELEASE) return;

    constexpr float step{5.0f};
    switch (key) {
      case GLENV_KEY_LEFT:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.y -= step;
        } else {
          rotateQuaternion({0.0f, 1.0f, 0.0f}, -step);
        }
        break;
      case GLENV_KEY_RIGHT:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.y += step;
        } else {
          rotateQuaternion({0.0f, 1.0f, 0.0f}, step);
        }
        break;
      case GLENV_KEY_UP:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.x -= step;
        } else {
          rotateQuaternion({1.0f, 0.0f, 0.0f}, -step);
        }
        break;
      case GLENV_KEY_DOWN:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.x += step;
        } else {
          rotateQuaternion({1.0f, 0.0f, 0.0f}, step);
        }
        break;
      case GLENV_KEY_Q:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.z -= step;
        } else {
          rotateQuaternion({0.0f, 0.0f, 1.0f}, -step);
        }
        break;
      case GLENV_KEY_E:
        if (rotationMode == RotationMode::Euler) {
          rotationAngles.z += step;
        } else {
          rotateQuaternion({0.0f, 0.0f, 1.0f}, step);
        }
        break;
      case GLENV_KEY_SPACE:
        if (action == GLENV_PRESS) {
          rotationMode = RotationMode{1 - int(rotationMode)};
        }
        break;
      case GLENV_KEY_R:
        resetRotation();
        break;
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
    }
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
        MessageBoxA(NULL, ss.str().c_str(), "OpenGL Error", MB_ICONERROR | MB_OK);
#endif
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
