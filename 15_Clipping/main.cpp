#include <ArcBall.h>
#include <GLApp.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <vector>

struct Vertex2D {
  float x;
  float y;
  float z;
  float inverseZ;
  Vec4 color;
};

struct Vertex3D {
  Vec3 position;
  Vec4 color;
};

struct VertexClip {
  Vec4 position;
  Vec4 color;
};

enum class ClipPlane {
  Left,
  Right,
  Bottom,
  Top,
  Near,
  Far
};

class MyGLApp : public GLApp {
public:
  Image image{800, 560, 4};
  std::vector<float> depthBuffer;
  ArcBall arcBall{Vec2ui{800, 600}};
  bool mouseDragActive{false};
  Mat4 cubeRotation{};
  Vec3 cubeTranslation{0.0f, 0.0f, 4.2f};

  constexpr static float focalLength{360.0f};
  constexpr static float nearPlane{0.8f};
  constexpr static float farPlane{8.0f};

  MyGLApp() : GLApp{800, 600, 1, "Exercise 15 - Clipping"} {
  }

  uint8_t toByte(const float value) const {
    return uint8_t(std::clamp(value, 0.0f, 1.0f) * 255.0f);
  }

  void clearImage(const Vec4& color) {
    for (uint32_t y = 0; y < image.height; ++y) {
      for (uint32_t x = 0; x < image.width; ++x) {
        const size_t index = image.computeIndex(x, y, 0);
        image.data[index + 0] = toByte(color.r);
        image.data[index + 1] = toByte(color.g);
        image.data[index + 2] = toByte(color.b);
        image.data[index + 3] = toByte(color.a);
      }
    }

    depthBuffer.assign(size_t(image.width) * size_t(image.height),
                       std::numeric_limits<float>::infinity());
  }

  void setPixel(const int x, const int y, const Vec4& color) {
    if (x < 0 || y < 0) return;
    if (x >= int(image.width) || y >= int(image.height)) return;

    const size_t index = image.computeIndex(uint32_t(x), uint32_t(y), 0);
    image.data[index + 0] = toByte(color.r);
    image.data[index + 1] = toByte(color.g);
    image.data[index + 2] = toByte(color.b);
    image.data[index + 3] = toByte(color.a);
  }

  void drawGrid() {
    for (int y = 20; y < int(image.height); y += 40) {
      for (uint32_t x = 0; x < image.width; ++x) {
        setPixel(int(x), y, {0.89f, 0.91f, 0.94f, 1.0f});
      }
    }

    for (int x = 20; x < int(image.width); x += 40) {
      for (uint32_t y = 0; y < image.height; ++y) {
        setPixel(x, int(y), {0.89f, 0.91f, 0.94f, 1.0f});
      }
    }
  }

  float edgeFunction(const Vertex2D& a,
                     const Vertex2D& b,
                     const float x,
                     const float y) const {
    return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
  }

  Vec4 interpolateColor(const Vertex2D& a,
                        const Vertex2D& b,
                        const Vertex2D& c,
                        const float lambdaA,
                        const float lambdaB,
                        const float lambdaC) const {
    const float inverseZ = lambdaA * a.inverseZ +
                           lambdaB * b.inverseZ +
                           lambdaC * c.inverseZ;
    if (inverseZ == 0.0f) {
      return {};
    }

    return (a.color * (lambdaA * a.inverseZ) +
            b.color * (lambdaB * b.inverseZ) +
            c.color * (lambdaC * c.inverseZ)) / inverseZ;
  }

  void drawTriangle(const Vertex2D& a, const Vertex2D& b, const Vertex2D& c) {
    float area = edgeFunction(b, c, a.x, a.y);
    if (std::abs(area) < 0.00001f) return;

    const float orientation = area < 0.0f ? -1.0f : 1.0f;
    area *= orientation;

    const int minX = std::max(0, int(std::floor(std::min({a.x, b.x, c.x}))));
    const int maxX = std::min(int(image.width) - 1,
                              int(std::ceil(std::max({a.x, b.x, c.x}))));
    const int minY = std::max(0, int(std::floor(std::min({a.y, b.y, c.y}))));
    const int maxY = std::min(int(image.height) - 1,
                              int(std::ceil(std::max({a.y, b.y, c.y}))));

    const float startX = float(minX) + 0.5f;
    const float startY = float(minY) + 0.5f;
    const float eAStart = orientation * edgeFunction(b, c, startX, startY);
    const float eBStart = orientation * edgeFunction(c, a, startX, startY);
    const float eCStart = orientation * edgeFunction(a, b, startX, startY);

    const float eADX = orientation * (c.y - b.y);
    const float eBDX = orientation * (a.y - c.y);
    const float eCDX = orientation * (b.y - a.y);
    const float eADY = orientation * -(c.x - b.x);
    const float eBDY = orientation * -(a.x - c.x);
    const float eCDY = orientation * -(b.x - a.x);

    float rowEA = eAStart;
    float rowEB = eBStart;
    float rowEC = eCStart;

    for (int y = minY; y <= maxY; ++y) {
      float eA = rowEA;
      float eB = rowEB;
      float eC = rowEC;

      for (int x = minX; x <= maxX; ++x) {
        if (eA >= 0.0f && eB >= 0.0f && eC >= 0.0f) {
          const float lambdaA = eA / area;
          const float lambdaB = eB / area;
          const float lambdaC = eC / area;
          const float inverseZ = lambdaA * a.inverseZ +
                                 lambdaB * b.inverseZ +
                                 lambdaC * c.inverseZ;
          if (inverseZ == 0.0f) {
            continue;
          }

          const float depth = 1.0f / inverseZ;
          const size_t depthIndex = size_t(x) + size_t(y) * size_t(image.width);
          if (depth < depthBuffer[depthIndex]) {
            depthBuffer[depthIndex] = depth;
            setPixel(x, y, interpolateColor(a, b, c, lambdaA, lambdaB, lambdaC));
          }
        }

        eA += eADX;
        eB += eBDX;
        eC += eCDX;
      }

      rowEA += eADY;
      rowEB += eBDY;
      rowEC += eCDY;
    }
  }

  float clipDistance(const VertexClip& vertex, const ClipPlane plane) const {
    const Vec4& p = vertex.position;

    switch (plane) {
      case ClipPlane::Left:   return p.x + p.w;
      case ClipPlane::Right:  return -p.x + p.w;
      case ClipPlane::Bottom: return p.y + p.w;
      case ClipPlane::Top:    return -p.y + p.w;
      case ClipPlane::Near:   return p.z + p.w;
      case ClipPlane::Far:    return -p.z + p.w;
    }

    return -1.0f;
  }

  VertexClip interpolateClipVertex(const VertexClip& a,
                                   const VertexClip& b,
                                   const float t) const {
    return {
      a.position + (b.position - a.position) * t,
      a.color + (b.color - a.color) * t
    };
  }

  Vec2ui arcBallPosition(const double xPosition, const double yPosition) const {
    const Dimensions winDim = glEnv.getWindowSize();
    const uint32_t maxX = winDim.width > 0 ? winDim.width - 1 : 0;
    const uint32_t maxY = winDim.height > 0 ? winDim.height - 1 : 0;
    const uint32_t x = uint32_t(std::clamp(xPosition, 0.0, double(maxX)));
    const uint32_t y = uint32_t(std::clamp(yPosition, 0.0, double(maxY)));
    return {maxX - x, y};
  }

  std::vector<VertexClip> clipPolygonAgainstPlane(const std::vector<VertexClip>& input,
                                                  const ClipPlane plane) const {
    // TODO Task 1:
    // Clip the input polygon against one clip-space halfspace.
    //
    // Use Sutherland-Hodgman clipping. Walk over every polygon edge from the
    // previous vertex to the current vertex. For each edge, decide whether the
    // start and end vertices are inside the current clip plane by evaluating
    // clipDistance(...). A distance >= 0 means inside.
    //
    // If an edge crosses the plane, compute the interpolation parameter with
    // t = dStart / (dStart - dEnd). Then use interpolateClipVertex(...) to
    // create the new vertex on the clipping plane. Remember to preserve and
    // interpolate all vertex attributes, not only the position.
    return input;
  }

  std::vector<VertexClip> clipTriangle(const VertexClip& a,
                                       const VertexClip& b,
                                       const VertexClip& c) const {
    // TODO Task 2:
    // Start with the three triangle vertices and clip the polygon successively
    // against all six planes of the canonical clip volume:
    //
    //   -w <= x <= w
    //   -w <= y <= w
    //   -w <= z <= w
    //
    // After clipping, the result may contain zero vertices, three vertices, or a
    // larger convex polygon. A larger polygon will be triangulated as a triangle
    // fan in drawCubeTriangle(...).
    return {a, b, c};
  }

  VertexClip projectToClip(const Vertex3D& vertex) const {
    const Vec3 cameraSpace = cubeRotation * vertex.position + cubeTranslation;
    const float sx = 2.0f * focalLength / float(image.width);
    const float sy = 2.0f * focalLength / float(image.height);
    const float a = (farPlane + nearPlane) / (farPlane - nearPlane);
    const float b = (-2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

    return {
      Vec4{cameraSpace.x * sx,
           cameraSpace.y * sy,
           cameraSpace.z * a + b,
           cameraSpace.z},
      vertex.color
    };
  }

  Vertex2D perspectiveDivide(const VertexClip& vertex) const {
    const float inverseW = 1.0f / vertex.position.w;
    const float ndcX = vertex.position.x * inverseW;
    const float ndcY = vertex.position.y * inverseW;

    return {
      (ndcX * 0.5f + 0.5f) * float(image.width),
      (0.5f - ndcY * 0.5f) * float(image.height),
      vertex.position.w,
      inverseW,
      vertex.color
    };
  }

  void drawCubeTriangle(const Vertex3D& a, const Vertex3D& b, const Vertex3D& c) {
    const std::vector<VertexClip> clipped =
      clipTriangle(projectToClip(a), projectToClip(b), projectToClip(c));

    for (size_t i = 1; i + 1 < clipped.size(); ++i) {
      drawTriangle(perspectiveDivide(clipped[0]),
                   perspectiveDivide(clipped[i]),
                   perspectiveDivide(clipped[i + 1]));
    }
  }

  void drawCube() {
    const std::array<Vec3, 8> positions{
      Vec3{-1.0f, -1.0f, -1.0f},
      Vec3{ 1.0f, -1.0f, -1.0f},
      Vec3{ 1.0f,  1.0f, -1.0f},
      Vec3{-1.0f,  1.0f, -1.0f},
      Vec3{-1.0f, -1.0f,  1.0f},
      Vec3{ 1.0f, -1.0f,  1.0f},
      Vec3{ 1.0f,  1.0f,  1.0f},
      Vec3{-1.0f,  1.0f,  1.0f}
    };

    const std::array<Vec4, 8> colors{
      Vec4{0.20f, 0.28f, 0.95f, 1.0f},
      Vec4{0.16f, 0.75f, 0.95f, 1.0f},
      Vec4{0.25f, 0.90f, 0.42f, 1.0f},
      Vec4{0.98f, 0.86f, 0.24f, 1.0f},
      Vec4{0.92f, 0.22f, 0.30f, 1.0f},
      Vec4{0.95f, 0.48f, 0.18f, 1.0f},
      Vec4{0.82f, 0.30f, 0.92f, 1.0f},
      Vec4{0.38f, 0.22f, 0.78f, 1.0f}
    };

    const auto vertex = [&](const size_t index) {
      return Vertex3D{positions[index], colors[index]};
    };

    drawCubeTriangle(vertex(4), vertex(5), vertex(6));
    drawCubeTriangle(vertex(4), vertex(6), vertex(7));
    drawCubeTriangle(vertex(1), vertex(0), vertex(3));
    drawCubeTriangle(vertex(1), vertex(3), vertex(2));
    drawCubeTriangle(vertex(0), vertex(4), vertex(7));
    drawCubeTriangle(vertex(0), vertex(7), vertex(3));
    drawCubeTriangle(vertex(5), vertex(1), vertex(2));
    drawCubeTriangle(vertex(5), vertex(2), vertex(6));
    drawCubeTriangle(vertex(3), vertex(7), vertex(6));
    drawCubeTriangle(vertex(3), vertex(6), vertex(2));
    drawCubeTriangle(vertex(0), vertex(1), vertex(5));
    drawCubeTriangle(vertex(0), vertex(5), vertex(4));
  }

  void renderScene() {
    clearImage({0.96f, 0.97f, 0.99f, 1.0f});
    drawGrid();
    drawCube();
  }

  void resetScene() {
    cubeRotation = Mat4{};
    cubeTranslation = {0.0f, 0.0f, 4.2f};
    renderScene();
  }

  virtual void init() override {
    setImageFilter(GL_NEAREST, GL_NEAREST);
    setAnimation(false);
    renderScene();
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);
    arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action == GLENV_RELEASE) return;

    constexpr float translateStep{0.25f};

    switch (key) {
      case GLENV_KEY_LEFT:
        cubeTranslation.x -= translateStep;
        renderScene();
        break;
      case GLENV_KEY_RIGHT:
        cubeTranslation.x += translateStep;
        renderScene();
        break;
      case GLENV_KEY_UP:
        cubeTranslation.z += translateStep;
        renderScene();
        break;
      case GLENV_KEY_DOWN:
        cubeTranslation.z -= translateStep;
        renderScene();
        break;
      case GLENV_KEY_R:
        resetScene();
        break;
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
    }
  }

  virtual void mouseButton(int button,
                           int state,
                           int mods,
                           double xPosition,
                           double yPosition) override {
    if (button != GLENV_MOUSE_BUTTON_LEFT) return;

    mouseDragActive = state == GLENV_MOUSE_PRESS;
    if (mouseDragActive) {
      arcBall.click(arcBallPosition(xPosition, yPosition));
    }
  }

  virtual void mouseMove(double xPosition, double yPosition) override {
    if (!mouseDragActive) return;

    cubeRotation =
      arcBall.drag(arcBallPosition(xPosition, yPosition)).computeRotation() *
      cubeRotation;
    renderScene();
  }

  virtual void draw() override {
    drawImage(image);
  }
} myApp;

#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
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
