#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include <ArcBall.h>
#include <GLApp.h>
#include <Tessellation.h>

namespace {

std::string shaderPath(const std::string& filename) {
  if (std::filesystem::exists(filename)) {
    return filename;
  }
  return std::string{"Shader/"} + filename;
}

Vec3 readVec3(const std::vector<float>& data, size_t index) {
  const size_t offset = index * 3;
  return Vec3{data[offset], data[offset + 1], data[offset + 2]};
}

void appendVec3(std::vector<float>& data, const Vec3& value) {
  data.push_back(value.x);
  data.push_back(value.y);
  data.push_back(value.z);
}

void appendVec4(std::vector<float>& data, const Vec4& value) {
  data.push_back(value.x);
  data.push_back(value.y);
  data.push_back(value.z);
  data.push_back(value.w);
}

Vec2ui mousePosition(double x, double y) {
  return Vec2ui{static_cast<uint32_t>(std::max(0.0, x)),
                static_cast<uint32_t>(std::max(0.0, y))};
}

struct Mesh {
  std::vector<float> triangles;
  std::vector<float> normalLines;
  Mat4 translation{};
  Mat4 localTransform{};
};

}

class LocalIlluminationApp : public GLApp {
public:
  LocalIlluminationApp()
    : GLApp(800, 600, 4, "Assignment 12 - Local Illumination"),
      arcBall(Vec2ui{800, 600}),
      phongProgram(GLProgram::createFromFile(shaderPath("vertexShader.vert"),
                                             shaderPath("fragmentShader.frag"),
                                             "", true, true))
  {}

  virtual void init() override {
    setBackground(0.04f, 0.045f, 0.055f, 1.0f);
    GL(glEnable(GL_DEPTH_TEST));

    setupMesh(cube,
              Tessellation::genBrick(Vec3{0.0f, 0.0f, 0.0f},
                                     Vec3{1.15f, 1.15f, 1.15f}).unpack(),
              Vec3{0.9f, 0.38f, 0.26f},
              Mat4::translation(-0.9f, 0.0f, 0.0f),
              Mat4{});

    setupMesh(sphere,
              Tessellation::genSphere(Vec3{0.0f, 0.0f, 0.0f},
                                      0.65f, 48, 24).unpack(),
              Vec3{0.22f, 0.58f, 0.96f},
              Mat4::translation(0.9f, 0.0f, 0.0f),
              Mat4::scaling(0.8f, 1.25f, 0.55f));

    updateTitle();
  }

  virtual void draw() override {
    const Vec3 viewPosition = cameraPosition();
    const Mat4 view = Mat4::lookAt(viewPosition, Vec3{0.0f, 0.0f, 0.0f},
                                  Vec3{0.0f, 1.0f, 0.0f});
    const Mat4 projection = Mat4::perspective(45.0f, getAspect(), 0.1f, 100.0f);
    const Vec3 lightPosition = lightRotation * baseLightPosition;

    drawMesh(cube, view, projection, lightPosition, viewPosition);
    drawMesh(sphere, view, projection, lightPosition, viewPosition);

    if (showNormals) {
      drawNormalLines(cube, view, projection);
      drawNormalLines(sphere, view, projection);
    }
    drawLightMarker(view, projection, lightPosition);
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);
    arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action != GLENV_PRESS) {
      return;
    }

    switch (key) {
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
      case GLENV_KEY_L:
        interactionMode = (interactionMode == InteractionMode::Objects)
        ? InteractionMode::Light
        : InteractionMode::Objects;
        updateTitle();
        break;
      case GLENV_KEY_N:
        showNormals = !showNormals;
        updateTitle();
        break;

      case GLENV_KEY_R:
        objectRotation = Mat4{};
        lightRotation = Mat4{};
        cameraDistance = defaultCameraDistance;
        sceneOffsetX = defaultSceneOffsetX;
        normalLineLength = defaultNormalLineLength;
        updateTitle();
        break;
      case GLENV_KEY_UP:
        cameraDistance = std::max(2.0f, cameraDistance * 0.9f);
        updateTitle();
        break;
      case GLENV_KEY_DOWN:
        cameraDistance = std::min(12.0f, cameraDistance * 1.1f);
        updateTitle();
        break;
      case GLENV_KEY_LEFT:
        sceneOffsetX = std::max(-2.0f, sceneOffsetX - 0.15f);
        updateTitle();
        break;
      case GLENV_KEY_RIGHT:
        sceneOffsetX = std::min(2.0f, sceneOffsetX + 0.15f);
        updateTitle();
        break;
      case GLENV_KEY_Q:
        normalLineLength = std::max(0.02f, normalLineLength * 0.85f);
        updateTitle();
        break;
      case GLENV_KEY_W:
        normalLineLength = std::min(0.6f, normalLineLength * 1.15f);
        updateTitle();
        break;
      default:
        break;
    }
  }

  virtual void mouseButton(int button, int state, int mods,
                           double xPosition, double yPosition) override {
    if (button != GLENV_MOUSE_BUTTON_LEFT) {
      return;
    }

    dragging = state == GLENV_MOUSE_PRESS;
    if (dragging) {
      arcBall.click(mousePosition(xPosition, yPosition));
    }
  }

  virtual void mouseMove(double xPosition, double yPosition) override {
    if (!dragging) {
      return;
    }

    const Mat4 dragRotation = arcBall.drag(mousePosition(xPosition, yPosition)).computeRotation();
    if (interactionMode == InteractionMode::Objects) {
      objectRotation = dragRotation * objectRotation;
    } else {
      lightRotation = dragRotation * lightRotation;
    }
  }

private:
  enum class InteractionMode {
    Objects,
    Light
  };

  ArcBall arcBall;
  GLProgram phongProgram;
  Mesh cube;
  Mesh sphere;

  bool dragging{false};
  bool showNormals{false};
  InteractionMode interactionMode{InteractionMode::Objects};

  Mat4 objectRotation{};
  Mat4 lightRotation{};
  const float defaultCameraDistance{5.0f};
  float cameraDistance{defaultCameraDistance};
  const float defaultSceneOffsetX{0.0f};
  float sceneOffsetX{defaultSceneOffsetX};
  const float defaultNormalLineLength{0.15f};
  float normalLineLength{defaultNormalLineLength};
  const Vec3 baseLightPosition{0.0f, 2.2f, 2.4f};

  Vec3 cameraPosition() const {
    return Vec3{0.0f, 0.2f, cameraDistance};
  }

  Mat4 sceneMatrix() const {
    return Mat4::translation(sceneOffsetX, 0.0f, 0.0f);
  }

  void setupMesh(Mesh& mesh, const Tessellation& tessellation,
                 const Vec3& color, const Mat4& translation,
                 const Mat4& localTransform) {
    const std::vector<float>& vertices = tessellation.getVertices();
    const std::vector<float>& normals = tessellation.getNormals();
    const size_t vertexCount = vertices.size() / 3;

    mesh.triangles.clear();
    mesh.triangles.reserve(vertexCount * 10);
    for (size_t i = 0; i < vertexCount; ++i) {
      appendVec3(mesh.triangles, readVec3(vertices, i));
      appendVec4(mesh.triangles, Vec4{color, 1.0f});
      appendVec3(mesh.triangles, readVec3(normals, i));
    }

    mesh.normalLines = buildNormalLines(tessellation);
    mesh.translation = translation;
    mesh.localTransform = localTransform;
  }

  std::vector<float> buildNormalLines(const Tessellation& tessellation) const {
    std::vector<float> lines;
    (void)tessellation;

    // TODO Aufgabe 1:
    // Build one short line segment per vertex. The start point should be the
    // vertex position, the direction should be the corresponding vertex normal.
    // Store the normal as a unit direction; its display length is applied in
    // drawNormalLines.
    // Store each line vertex as position (x, y, z) followed by color (r, g, b, a).

    return lines;
  }

  Mat4 modelMatrix(const Mesh& mesh) const {
    return sceneMatrix() * mesh.translation * objectRotation * mesh.localTransform;
  }

  void drawMesh(const Mesh& mesh, const Mat4& view,
                const Mat4& projection, const Vec3& lightPosition,
                const Vec3& viewPosition) {
    const Mat4 model = modelMatrix(mesh);
    const Mat4 normalMatrix = Mat4::transpose(Mat4::inverse(model));

    phongProgram.enable();
    phongProgram.setUniform("modelMatrix", model);
    phongProgram.setUniform("normalMatrix", normalMatrix);
    phongProgram.setUniform("viewMatrix", view);
    phongProgram.setUniform("projectionMatrix", projection);
    phongProgram.setUniform("lightPosition", lightPosition);
    phongProgram.setUniform("viewPosition", viewPosition);
    phongProgram.setUniform("ambientLight", Vec3{0.12f, 0.12f, 0.12f});
    phongProgram.setUniform("diffuseLight", Vec3{1.0f, 0.94f, 0.84f});
    phongProgram.setUniform("specularLight", Vec3{1.0f, 1.0f, 1.0f});
    phongProgram.setUniform("shininess", 32.0f);

    drawTriangles(mesh.triangles, TrisDrawType::LIST, false, phongProgram);
  }

  void drawNormalLines(const Mesh& mesh, const Mat4& view, const Mat4& projection) {
    (void)mesh;
    (void)view;
    (void)projection;

    // TODO Aufgabe 1:
    // Transform the line start point with the model-view matrix and the normal
    // direction with transpose(inverse(modelView)). Scale the transformed direction
    // by normalLineLength and draw the resulting view space lines with drawLines.
  }

  void drawLightMarker(const Mat4& view, const Mat4& projection, const Vec3& lightPosition) {
    const std::vector<float> lightPoint{
      lightPosition.x, lightPosition.y, lightPosition.z,
      1.0f, 0.9f, 0.25f, 1.0f
    };

    setDrawProjection(projection);
    setDrawTransform(view);
    drawPoints(lightPoint, 14.0f, true);
  }

  void updateTitle() {
    std::stringstream title;
    title << "Assignment 12 - Local Illumination | Mode: "
    << (interactionMode == InteractionMode::Objects ? "objects" : "light")
    << " | normals: " << (showNormals ? "on" : "off")
    << " | L: mode, N: normals, Up/Down: zoom, Left/Right: pan, Q/W: normal length, R: reset";
    glEnv.setTitle(title.str());
  }
} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
  std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
  std::vector<std::string> args{argv + 1, argv + argc};
#endif
  try {
    myApp.run();
  } catch (const GLException& e) {
    std::stringstream ss;
    ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
    std::cerr << ss.str() << std::endl;
#else
    MessageBoxA(nullptr, ss.str().c_str(), "OpenGL Error", MB_ICONERROR | MB_OK);
#endif
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
