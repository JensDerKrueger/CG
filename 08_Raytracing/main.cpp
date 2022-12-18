#include <GLApp.h>
#include <cmath>
#include <optional>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"

class MyGLApp : public GLApp {
public:
  Image image{600,600};
  
  MyGLApp() : GLApp{600,600,1,"Raytrace Demo"} {}
    
  virtual void init() {
    GL(glDisable(GL_CULL_FACE));
    GL(glClearColor(0,0,0,0));
    render(Scene::genSimpleScene(), 9);
  }

  void render(Scene scene, int depth) {
      Camera camera;
      camera.setEyePoint(Vec3{ 0.0, 0.0, 2.0 });
      camera.setLookAt(Vec3{ 0.0, 0.0, 0.0 });

      // instantiate the actual renderer with recursion depth 9 and 9x super sampling
      // NOTE: you might want to reduce the super sampling to 1 for testing and debugging
      Raytracer renderer(depth, 9);
      renderer.setCamera(camera);
      renderer.setScene(scene);
      renderer.render(image);
  }

  virtual void draw() {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    drawImage(image);
  }

} myApp;

int main(int argc, char ** argv) {
  myApp.run();
  return EXIT_SUCCESS;
}  
