#include <ArcBall.h>
#include <GLApp.h>
#include <cmath>
#include <optional>

class MyGLApp : public GLApp {
public:
  Image image{800,800};
  ArcBall arcBall{Vec2ui{800,800}};
  bool mouseDragActive{false};
  Mat4 lightRotation{};
  const Vec3 baseLightPos{0.0f,4.0f,0.0f};
  
  MyGLApp() : GLApp{800,800,1,"Intersection Demo"} {}

  std::optional<Vec3> raySphereIntersect(const Vec3& sphereCenter,
                                         const float& radius,
                                         const Vec3& rayOrigin,
                                         const Vec3& pixelPos) {
    // TODO:
    // Implement a ray/sphere intersection here.
    // The sphere is given as sphereCenter and radius.
    // The ray starts at rayOrigin and passes through pixelPos.
    // If no intersection is found, simply return {}. Otherwise,
    // return the intersection position.
    // If the function works correctly, you should see a glossy
    // red sphere, illuminated from the top front.

    return {};
  }
  
  Vec3 computeLighting(const Vec3& rayOrigin, const Vec3& lightPos, const Vec3& intersectionPoint, const Vec3& normal,
                       const Vec3& specularColor, const Vec3& diffuseColor, const Vec3& ambientColor) {
    const Vec3 viewDir  = Vec3::normalize(rayOrigin-intersectionPoint);
    const Vec3 lightDir = Vec3::normalize(lightPos-intersectionPoint);
    const Vec3 reflectedDir = normal * 2.0f * Vec3::dot(normal, lightDir) - lightDir;
    
    const Vec3 specular = specularColor * pow(std::max(0.0f,Vec3::dot(reflectedDir, viewDir)),16.0f);
    const Vec3 diffuse  = diffuseColor * std::max(0.0f,Vec3::dot(normal, lightDir));
    const Vec3 ambient  = ambientColor;
    return specular + diffuse + ambient;
  }

  void renderImage() {
    const Vec3 lightPos = lightRotation * baseLightPos;
    const Vec3 sphereCenter{0.0f, 0.0f, -4.0f};
    const float radius = 2.0f;
    const Vec3 rayOrigin{0.0f, 0.0f, 4.0f};
    const Vec3 topLeftCorner{-2.0f, 2.0f, 0.0f}, topRightCorner{2.0f, 2.0f, 0.0f};
    const Vec3 bottomLeftCorner{-2.0f, -2.0f, 0.0f}, bottomRightCorner{2.0f, -2.0f, 0.0f};
    const Vec3 deltaX = (topRightCorner-topLeftCorner)/ float(image.width);
    const Vec3 deltaY = (topRightCorner-bottomRightCorner)/ float(image.height);
    
    for (uint32_t y = 0;y< uint32_t(image.height);++y) {
      for (uint32_t x = 0;x< uint32_t(image.width);++x) {
        const Vec3 pixelPos = bottomLeftCorner + deltaX*float(x) + deltaY*float(y);
        const std::optional<Vec3> intersection = raySphereIntersect(sphereCenter, radius, rayOrigin, pixelPos);
        Vec3 color;
        if (intersection) {
          const Vec3 normal=(*intersection - sphereCenter) / radius;
          color = computeLighting(rayOrigin, lightPos, *intersection, normal,
                                             Vec3{1.0f,1.0f,1.0f}, Vec3{1.0f,0.0f,0.0f}, Vec3{0.1f,0.0f,0.0f});
        } else {
          color = Vec3{0.0f,0.0f,0.0f};
        }
        image.setNormalizedValue(x,y,0,color.r);
        image.setNormalizedValue(x,y,1,color.g);
        image.setNormalizedValue(x,y,2,color.b);
        image.setValue(x,y,3,255);
      }
    }
  }
    
  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glClearColor(0,0,0,0));
    renderImage();
  }
    
  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    drawImage(image);
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);
    arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action != GLENV_PRESS) return;

    switch (key) {
      case GLENV_KEY_R:
        lightRotation = Mat4{};
        renderImage();
        break;
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
      default:
        break;
    }
  }

  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) override {
    if (button != GLENV_MOUSE_BUTTON_LEFT) return;

    mouseDragActive = state == GLENV_MOUSE_PRESS;
    if (mouseDragActive) {
      arcBall.click(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)});
    }
  }

  virtual void mouseMove(double xPosition, double yPosition) override {
    if (!mouseDragActive) return;

    lightRotation = arcBall.drag(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)}).computeRotation() * lightRotation;
    renderImage();
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
