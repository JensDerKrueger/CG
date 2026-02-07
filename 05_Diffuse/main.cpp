#include <GLApp.h>

#include "Triangle.h"
#include "AmbientShader.h"
#include "DiffuseShader.h"

class MyGLApp : public GLApp
{
public:
  Image image{ 600, 600 };

  MyGLApp() : GLApp{ 600, 600, 1, "Diffuse Lighting"} {}

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));

    // define some materials for our triangles
    const Material red{ Vec3{1, 0, 0} };
    const Material green{ Vec3{0, 1, 0} };
    const Material blue{ Vec3{0, 0, 1} };
    const Material lightOrange{ Vec3{1, 0.765f, 0.482f } };

    // define the vertices for the first triangle
    const Vertex v0{ Vec3{12, 360, 0}, red };
    const Vertex v1{ Vec3{240, 588, 0}, green };
    const Vertex v2{ Vec3{444, 48, 0}, blue };

    // create a simple shader for the first triangle
    const AmbientShader as{};

    // create and draw the first triangle
    Triangle triangle1(v0, v1, v2, as);
    triangle1.draw(image);

    // define new vertices for the second triangle
    const Vertex d0{ Vec3{12, 384, 0}, lightOrange };
    const Vertex d1{ Vec3{216, 588, 0}, lightOrange };
    const Vertex d2{ Vec3{12, 588, 0}, lightOrange };

    // create a diffuse shader for the second triangle
    Vec3 lightPos{ 12, 486, 50 };
    const Vec3 light_diffuse_color{ 1, 1, 1 };
    const DiffuseShader diffuseShader{ lightPos, light_diffuse_color };

    // create and draw the diffuse shaded triangle
    Triangle triangle2(d0, d1, d2, diffuseShader);
    triangle2.draw(image);
  }

  virtual void draw() override {
    drawImage(image);
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