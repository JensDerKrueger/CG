#include <GLApp.h>

#include <algorithm>
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
    return a.color * lambdaA + b.color * lambdaB + c.color * lambdaC;
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
    // 8. Use the barycentric coordinates to interpolate depth and color.
    // 9. Use depthBuffer to keep only the closest triangle at every pixel.
  }

  Vertex2D animatedVertex(Vertex2D vertex, const size_t index) const {
    vertex.x += 22.0f * std::sin(animationPhase * 0.75f + float(index) * 1.4f);
    vertex.y += 16.0f * std::cos(animationPhase * 0.95f + float(index) * 1.1f);
    return vertex;
  }

  void renderScene() {
    clearImage({0.98f, 0.98f, 0.96f, 1.0f});
    drawGrid();

    drawTriangle(animatedVertex({95.0f, 88.0f, 0.72f, {0.95f, 0.50f, 0.35f, 1.0f}}, 0),
                 animatedVertex({600.0f, 140.0f, 0.72f, {0.98f, 0.82f, 0.30f, 1.0f}}, 1),
                 animatedVertex({225.0f, 485.0f, 0.72f, {0.70f, 0.40f, 0.90f, 1.0f}}, 2));

    drawTriangle(animatedVertex({285.0f, 80.0f, 0.35f, {0.24f, 0.54f, 0.96f, 1.0f}}, 3),
                 animatedVertex({720.0f, 410.0f, 0.35f, {0.26f, 0.84f, 0.72f, 1.0f}}, 4),
                 animatedVertex({215.0f, 430.0f, 0.35f, {0.25f, 0.70f, 0.38f, 1.0f}}, 5));

    drawTriangle(animatedVertex({470.0f, 45.0f, 0.18f, {0.96f, 0.30f, 0.25f, 1.0f}}, 6),
                 animatedVertex({760.0f, 500.0f, 0.18f, {0.96f, 0.62f, 0.28f, 1.0f}}, 7),
                 animatedVertex({385.0f, 475.0f, 0.18f, {0.88f, 0.25f, 0.58f, 1.0f}}, 8));
  }

  virtual void init() override {
    setImageFilter(GL_NEAREST, GL_NEAREST);
    setAnimation(false);
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
