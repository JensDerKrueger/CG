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

class MyGLApp : public GLApp {
public:
  Image image{800, 560, 4};
  std::vector<float> depthBuffer;
  float animationPhase{0.0f};

  MyGLApp() : GLApp{800, 600, 1, "Exercise 14 - Triangle Rasterization"} {
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
    depthBuffer.assign(size_t(image.width) * size_t(image.height), std::numeric_limits<float>::infinity());
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

  float edgeFunction(const Vertex2D& a, const Vertex2D& b, const float x, const float y) const {
    // TODO Task 1:
    // Return the signed edge function value for the edge a->b and the point
    // p = (x, y). This is the signed area of the parallelogram spanned by
    // (b - a) and (p - a).
    //
    // The sign tells you on which side of the directed edge the point lies. The
    // function is linear in x and y, which is why it can later be updated by
    // additions in the rasterization loop.
    return 0.0f;
  }

  Vec4 interpolateColor(const Vertex2D& a,
                        const Vertex2D& b,
                        const Vertex2D& c,
                        const float lambdaA,
                        const float lambdaB,
                        const float lambdaC) const {
    const float inverseZ = lambdaA * a.inverseZ + lambdaB * b.inverseZ + lambdaC * c.inverseZ;
    if (inverseZ == 0.0f) {
      return {};
    }

    return (a.color * (lambdaA * a.inverseZ) +
            b.color * (lambdaB * b.inverseZ) +
            c.color * (lambdaC * c.inverseZ)) / inverseZ;
  }

  void drawTriangle(const Vertex2D& a, const Vertex2D& b, const Vertex2D& c) {
    // TODO Task 2:
    // Rasterize the triangle efficiently using a bounding box and edge
    // functions.
    //
    // 1. Compute the signed triangle area using one edge function. Return for
    //    degenerate triangles with near-zero area.
    // 2. Accept both clockwise and counter-clockwise triangles by introducing an
    //    orientation factor. After applying it, the inside test can consistently
    //    use edgeValue >= 0.
    // 3. Compute the screen-space bounding box of the triangle and clamp it to
    //    the image dimensions. This is the first efficiency improvement compared
    //    to testing every pixel of the whole image.
    // 4. Evaluate the three edge functions at the center of the first pixel in
    //    the bounding box.
    // 5. Compute the constant x and y increments for each edge function. Moving
    //    from one pixel to the next should update the edge values by addition,
    //    not by recomputing the full formula.
    // 6. Loop over all pixels in the bounding box. If all three edge values are
    //    non-negative, the pixel center lies inside the triangle.
    // 7. Normalize the edge values by the triangle area to obtain barycentric
    //    coordinates.
    // 8. Use the barycentric coordinates to interpolate depth and color
    //    perspective-correctly. The projected vertices already store inverseZ.
    // 9. Use depthBuffer to keep only the closest triangle at every pixel.
  }

  Vec3 rotateCubeVertex(const Vec3& vertex) const {
    const float angleX = animationPhase * 0.75f + 0.55f;
    const float angleY = animationPhase * 1.10f + 0.70f;
    const float cosX = std::cos(angleX);
    const float sinX = std::sin(angleX);
    const float cosY = std::cos(angleY);
    const float sinY = std::sin(angleY);

    const Vec3 rotatedX{
      vertex.x,
      vertex.y * cosX - vertex.z * sinX,
      vertex.y * sinX + vertex.z * cosX
    };

    return Vec3{
      rotatedX.x * cosY + rotatedX.z * sinY,
      rotatedX.y,
      -rotatedX.x * sinY + rotatedX.z * cosY
    };
  }

  Vertex2D projectVertex(const Vertex3D& vertex) const {
    const Vec3 rotated = rotateCubeVertex(vertex.position);
    const Vec3 cameraSpace{rotated.x, rotated.y, rotated.z + 4.2f};
    const float focalLength = 360.0f;
    const float inverseZ = 1.0f / cameraSpace.z;

    return Vertex2D{
      float(image.width) * 0.5f + cameraSpace.x * focalLength * inverseZ,
      float(image.height) * 0.5f - cameraSpace.y * focalLength * inverseZ,
      cameraSpace.z,
      inverseZ,
      vertex.color
    };
  }

  void drawCubeTriangle(const Vertex3D& a, const Vertex3D& b, const Vertex3D& c) {
    drawTriangle(projectVertex(a), projectVertex(b), projectVertex(c));
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

  virtual void init() override {
    setImageFilter(GL_NEAREST, GL_NEAREST);
    setAnimation(true);
    renderScene();
  }

  virtual void animate(double animationTime) override {
    animationPhase = float(animationTime);
    renderScene();
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action != GLENV_PRESS) return;

    switch (key) {
      case GLENV_KEY_SPACE:
        setAnimation(!getAnimation());
        break;
      case GLENV_KEY_R:
        resetAnimation();
        animationPhase = 0.0f;
        renderScene();
        break;
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
    }
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
