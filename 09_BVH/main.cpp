#include <GLApp.h>
#include <ArcBall.h>
#include <FontRenderer.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"

enum class RenderMode
{
  SplitScreen,
  FullBVH,
  FullBruteForce
};

class MyGLApp : public GLApp {
public:
  static constexpr uint32_t PIXELS_PER_BATCH = 32;
  static constexpr uint32_t DEFAULT_BVH_DEBUG_DEPTH = 12;
  static constexpr uint32_t MAX_BVH_DEBUG_DEPTH = 32;
  static constexpr float ZOOM_STEP = 0.1f;
  static constexpr float MIN_ZOOM_DISTANCE = 0.4f;
  static constexpr float MAX_ZOOM_DISTANCE = 20.0f;
  static constexpr double TIME_BUDGET_MS_PER_RENDERER = 8.0;

  Image image{600,600};
  Scene scene;
  Camera camera;
  Raytracer bvhRenderer{9, 4};
  Raytracer bruteForceRenderer{9, 4};
  FontRenderer fontRenderer{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fontEngine{nullptr};
  std::vector<float> previewData;
  std::vector<float> bvhLineData;
  float lineThickness{1.0f};
  Vec3 previewCenter;
  ArcBall arcBall{Vec2ui{600, 600}};
  bool mouseDragActive{false};
  bool showPreview{true};
  bool showBVH{true};
  bool autoRenderOnMouseRelease{true};
  bool renderRequested{false};
  RenderMode renderMode{RenderMode::FullBVH};
  bool bvhRenderFinished{true};
  bool bruteForceRenderFinished{true};
  uint32_t bvhDebugDepth{DEFAULT_BVH_DEBUG_DEPTH};
  uint64_t bvhNextPixel{0};
  uint64_t bruteForceNextPixel{0};

  MyGLApp() : GLApp{600,600,1,"BVH Demo"} {}

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glEnable(GL_DEPTH_TEST));
    fontEngine = fontRenderer.generateFontEngine();

    camera.setEyePoint(Vec3{ 0.0, 0.0, 2.0 });
    camera.setLookAt(Vec3{ 0.0, 0.0, 0.0 });

    scene = Scene::genSimpleScene();
    const Vec3 backgroundColor = scene.getBackgroundcolor();
    setBackground(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
    previewData = scene.getTriangleData();
    bvhLineData = scene.getBVHLineData(bvhDebugDepth);
    previewCenter = computePreviewCenter(previewData);
    requestRender();
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override
  {
    GLApp::resize(winDim, fbDim);
    arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
  }

  Vec3 computePreviewCenter(const std::vector<float>& data) const
  {
    Vec3 center;
    size_t vertexCount = 0;

    for (size_t i = 0; i + 9 < data.size(); i += 10)
    {
      center = center + Vec3{data[i + 0], data[i + 1], data[i + 2]};
      ++vertexCount;
    }

    return vertexCount == 0 ? Vec3{} : center / float(vertexCount);
  }

  void requestRender()
  {
    renderRequested = true;
  }

  void cycleRenderMode()
  {
    switch (renderMode) {
      case RenderMode::SplitScreen:
        renderMode = RenderMode::FullBVH;
        break;
      case RenderMode::FullBVH:
        renderMode = RenderMode::FullBruteForce;
        break;
      case RenderMode::FullBruteForce:
        renderMode = RenderMode::SplitScreen;
        break;
    }

    requestRender();
  }

  void setBVHDebugDepth(uint32_t debugDepth)
  {
    bvhDebugDepth = std::min(debugDepth, MAX_BVH_DEBUG_DEPTH);
    bvhLineData = scene.getBVHLineData(bvhDebugDepth);
  }

  void increaseBVHDebugDepth()
  {
    setBVHDebugDepth(bvhDebugDepth + 1);
  }

	  void decreaseBVHDebugDepth()
	  {
	    if (bvhDebugDepth > 0)
	      setBVHDebugDepth(bvhDebugDepth - 1);
	  }

  void zoom(float delta)
  {
    const Vec3 eyePoint = camera.getEyePoint();
    const Vec3 viewDir = camera.getViewDir();
    const Vec3 newEyePoint = eyePoint + viewDir * delta;
    const float distanceToCenter = (previewCenter - newEyePoint).length();
    if (distanceToCenter < MIN_ZOOM_DISTANCE || distanceToCenter > MAX_ZOOM_DISTANCE)
      return;

    camera.setEyePoint(newEyePoint);
    camera.setLookAt(previewCenter);
    showPreview = true;
    if (autoRenderOnMouseRelease && !mouseDragActive)
      requestRender();
  }

  uint32_t splitX() const
  {
    return image.width / 2;
  }

  void clearRenderImage()
  {
    const Vec3 background = scene.getBackgroundcolor();
    for (uint32_t y = 0; y < image.height; ++y) {
      for (uint32_t x = 0; x < image.width; ++x) {
        image.setNormalizedValue(x, y, 0, background.r);
        image.setNormalizedValue(x, y, 1, background.g);
        image.setNormalizedValue(x, y, 2, background.b);
        image.setValue(x, y, 3, 255);
      }
    }
  }

  void startRenderIfNeeded()
  {
    if (!renderRequested || mouseDragActive)
      return;

    renderRequested = false;
    showPreview = false;
    bvhNextPixel = 0;
    bruteForceNextPixel = 0;
    bvhRenderFinished = false;
    bruteForceRenderFinished = false;
    clearRenderImage();

    bvhRenderer.setCamera(camera);
    bvhRenderer.setScene(scene);
    bvhRenderer.setUseBVH(true);
    bvhRenderer.setScanDirection(ScanDirection::TopToBottom);
    bruteForceRenderer.setCamera(camera);
    bruteForceRenderer.setScene(scene);
    bruteForceRenderer.setUseBVH(false);
    bruteForceRenderer.setScanDirection(ScanDirection::TopToBottom);
  }

  void renderProgressiveStep()
  {
    startRenderIfNeeded();
    if (showPreview || mouseDragActive)
      return;

    switch (renderMode) {
      case RenderMode::SplitScreen: {
        const uint32_t middle = splitX();
        renderPixelsForTime(bvhRenderer, 0, middle, bvhNextPixel, PIXELS_PER_BATCH, bvhRenderFinished);
        renderPixelsForTime(bruteForceRenderer, middle, image.width, bruteForceNextPixel, 1, bruteForceRenderFinished);
        break;
      }
      case RenderMode::FullBVH:
        renderPixelsForTime(bvhRenderer, 0, image.width, bvhNextPixel, PIXELS_PER_BATCH, bvhRenderFinished);
        break;
      case RenderMode::FullBruteForce:
        renderPixelsForTime(bruteForceRenderer, 0, image.width, bruteForceNextPixel, PIXELS_PER_BATCH, bruteForceRenderFinished);
        break;
    }
  }

  uint64_t pixelCount(uint32_t xBegin, uint32_t xEnd) const
  {
    return uint64_t(xEnd - xBegin) * image.height;
  }

  uint64_t totalPixelCount() const
  {
    return pixelCount(0, image.width);
  }

  void renderPixelsForTime(Raytracer& renderer, uint32_t xBegin, uint32_t xEnd, uint64_t& nextPixel, uint32_t pixelsPerBatch, bool& finished)
  {
    if (finished)
      return;

    const uint64_t totalPixels = pixelCount(xBegin, xEnd);
    const auto startTime = std::chrono::steady_clock::now();
    while (!finished) {
      const uint64_t remainingPixels = totalPixels > nextPixel ? totalPixels - nextPixel : 0;
      if (remainingPixels == 0) {
        finished = true;
        return;
      }

      const uint32_t batchSize = uint32_t(std::min<uint64_t>(pixelsPerBatch, remainingPixels));
      finished = renderer.renderPixels(image, xBegin, xEnd, nextPixel, batchSize);

      const std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - startTime;
      if (elapsed.count() >= TIME_BUDGET_MS_PER_RENDERER)
        return;
    }
  }

  void updatePreviewLight()
  {
    std::shared_ptr<const LightSource> light = scene.getLight(0);
    if (!light)
      return;

    const Vec3 localOrigin;
    const Vec3 localLightPos = localOrigin + light->getDirection(localOrigin) * light->getDistance(localOrigin);
    setLightPos(scene.getModel() * localLightPos);
  }

  void drawPreviewText()
  {
    if (!fontEngine)
      return;

    fontEngine->render("BVH Preview", getAspect(), 0.035f, {0.0f, -0.92f}, Alignment::Center, {1.0f, 0.0f, 0.0f, 0.9f});
  }

  void drawBVHOverlay()
  {
    if (!showBVH || bvhLineData.empty())
      return;

    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthMask(GL_FALSE));

    setDrawProjection(camera.getViewProjection(getAspect()));
    setDrawTransform(scene.getModel());
    drawLines(bvhLineData, LineDrawType::LIST, lineThickness);

    GL(glDepthMask(GL_TRUE));
    GL(glEnable(GL_DEPTH_TEST));
  }

  uint32_t renderPercent(uint64_t nextPixel, uint64_t totalPixels) const
  {
    if (totalPixels == 0)
      return 100;

    return uint32_t(std::min<uint64_t>(100, nextPixel * 100 / totalPixels));
  }

  virtual void draw() override {
    renderProgressiveStep();

    if (showPreview)
    {
      setDrawProjection(camera.getViewProjection(getAspect()));
      setDrawTransform(scene.getModel());
      updatePreviewLight();
      drawTriangles(previewData, TrisDrawType::LIST, false, true);
      drawBVHOverlay();
      drawPreviewText();
    }
    else
    {
      setDrawProjection(Mat4{});
      setDrawTransform(Mat4{});
      drawImage(image);
      drawBVHOverlay();
    }
  }

  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) override
  {
    if (button != GLENV_MOUSE_BUTTON_LEFT)
      return;

    mouseDragActive = state == GLENV_MOUSE_PRESS;
    if (mouseDragActive)
    {
      arcBall.click(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)});
      showPreview = true;
      if (!autoRenderOnMouseRelease)
        renderRequested = false;
    }
    else if (autoRenderOnMouseRelease)
    {
      requestRender();
    }
  }

  virtual void mouseMove(double xPosition, double yPosition) override
  {
    if (!mouseDragActive)
      return;

    const Mat4 rotation = arcBall.drag(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)}).computeRotation();
    const Vec3 pivot = scene.getModel() * previewCenter;
    const Mat4 rotateAroundPivot = Mat4::translation(pivot) * rotation * Mat4::translation(pivot * -1.0f);
    scene.setModel(rotateAroundPivot * scene.getModel());
    if (autoRenderOnMouseRelease)
      requestRender();
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override
  {
    if (action != GLENV_PRESS)
      return;

    switch (key) {
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
      case GLENV_KEY_B:
        showBVH = !showBVH;
        break;
      case GLENV_KEY_M:
        cycleRenderMode();
        break;
      case GLENV_KEY_RIGHT:
        increaseBVHDebugDepth();
        break;
      case GLENV_KEY_LEFT:
        decreaseBVHDebugDepth();
        break;
      case GLENV_KEY_UP:
        zoom(ZOOM_STEP);
        break;
      case GLENV_KEY_DOWN:
        zoom(-ZOOM_STEP);
        break;
      case GLENV_KEY_W:
        lineThickness += 1.0f;
        break;
      case GLENV_KEY_Q:
        lineThickness -= 1.0f;
        break;
      case GLENV_KEY_R:
        autoRenderOnMouseRelease = !autoRenderOnMouseRelease;
        if (autoRenderOnMouseRelease && !mouseDragActive)
          requestRender();
        break;
      default:
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
