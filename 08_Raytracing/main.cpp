#include <GLApp.h>
#include <ArcBall.h>
#include <FontRenderer.h>
#include <BackgroundTask.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"

class MyGLApp : public GLApp {
public:
  Image image{600,600};
  Scene scene;
  Camera camera;
  FontRenderer fontRenderer{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fontEngine{nullptr};
  std::vector<float> previewData;
  Vec3 previewCenter;
  ArcBall arcBall{Vec2ui{600, 600}};
  bool mouseDragActive{false};
  bool showPreview{true};
  BackgroundTask<Image> renderTask;

  MyGLApp() : GLApp{600,600,1,"Raytrace Demo"} {}

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
    previewCenter = computePreviewCenter(previewData);
    requestRender();
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override
  {
    GLApp::resize(winDim, fbDim);
    arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
  }

  static Image render(Scene scene, Camera camera, int depth, uint32_t width, uint32_t height)
  {
    Image result{width, height};
    Raytracer renderer(depth, 9);
    renderer.setCamera(camera);
    renderer.setScene(scene);
    renderer.render(result);
    return result;
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
    renderTask.request();
    showPreview = true;
  }

  void startRenderIfNeeded()
  {
    if (mouseDragActive || !renderTask.canStart())
      return;

    const Scene sceneToRender = scene;
    const Camera cameraToRender = camera;
    const uint32_t width = image.width;
    const uint32_t height = image.height;

    renderTask.start([sceneToRender, cameraToRender, width, height]() {
      return render(sceneToRender, cameraToRender, 9, width, height);
    });
  }

  void collectRenderResult()
  {
    if (renderTask.takeResult(image))
      showPreview = false;
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

    fontEngine->render("OpenGL Preview", getAspect(), 0.035f, {0.0f, -0.92f}, Alignment::Center, {1.0f, 0.0f, 0.0f, 0.9f});
  }

  virtual void draw() override {
    collectRenderResult();
    startRenderIfNeeded();
    collectRenderResult();

    if (showPreview)
    {
      setDrawProjection(camera.getViewProjection(getAspect()));
      setDrawTransform(scene.getModel());
      updatePreviewLight();
      drawTriangles(previewData, TrisDrawType::LIST, false, true);
      drawPreviewText();
    }
    else
    {
      setDrawProjection(Mat4{});
      setDrawTransform(Mat4{});
      drawImage(image);
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
    }
    else
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
    requestRender();
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
