#include <GLApp.h>
#include <OBJFile.h>
#include <Mat4.h>

class MyGLApp : public GLApp {
public:
  double angle{0.0};
  std::vector<float> data;
  
  MyGLApp() : GLApp{640,480,1,"Shared vertices to explicit representation demo"} {}
  
  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glEnable(GL_DEPTH_TEST));

    const OBJFile m{"bunny.obj", true};

    // TODO:
    // Replace this example block of code  by your code to
    // convert the shared vertex representation in the object m
    // by an explicit representation, indices are stored in
    // the stl-vector m.indices the vertex positions are stored
    // in m.vertices and the normals are stored in m.normals.
    // As color you can choose whatever you like
    
    data.push_back(0.0f); data.push_back(0.5f); data.push_back(0.0f);  // position
    data.push_back(1.0f); data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); // color
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); // normal

    data.push_back(-0.5f); data.push_back(-0.5f); data.push_back(0.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); data.push_back(1.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);

    data.push_back(0.5f); data.push_back(-0.5f); data.push_back(0.0f);
    data.push_back(0.0f); data.push_back(1.0f); data.push_back(0.0f); data.push_back(1.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);

    // example block end
  }
  
  virtual void animate(double animationTime) override {
    angle = animationTime*30;
  }
  
  virtual void draw() override {
    setDrawProjection(Mat4::perspective(45, glEnv.getFramebufferSize().aspect(), 0.0001f, 100.0f));
    setDrawTransform(Mat4::lookAt({0,0,2},{0,0,0},{0,1,0}) * Mat4::rotationY(float(angle)));
    drawTriangles(data, TrisDrawType::LIST, false, true);
  }
  
  virtual void keyboardChar(unsigned int key) override {
    // TODO check for keystrokes here
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
