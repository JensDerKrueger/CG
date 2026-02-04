#define showOctree
//#define only2D

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>

#include <GLApp.h>

#include <ColorConversion.h>
#include <Vec3.h>
#include <Mat4.h>
#include <Rand.h>

#include "Octree.h"
#include <AbstractParticleSystem.h>

class SimpleStaticParticleSystem : public AbstractParticleSystem {
  public:
  SimpleStaticParticleSystem(const std::vector<Vec3> data, float pointSize) :
  AbstractParticleSystem(pointSize),
  color{ 1.0f,1.0f,1.0f }
  {
    setData(data);
  }

  virtual ~SimpleStaticParticleSystem() {}

  virtual void update(float t) {}
  virtual void setColor(const Vec3& color) { this->color = color; }

  void setData(const std::vector<float> data)
  {
    floatParticleData.resize(data.size() / 3 * 7);
    size_t j = 0;
    for (size_t i = 0; i < data.size(); i += 3) {
      floatParticleData[j * 7 + 0] = data[i + 0];
      floatParticleData[j * 7 + 1] = data[i + 1];
      floatParticleData[j * 7 + 2] = data[i + 2];

      Vec3 c = color == RAINBOW_COLOR ? ColorConversion::hsvToRgb(Vec3{ float(i) / data.size() * 360.0f,1.0f,1.0f }) : computeColor(color);

      floatParticleData[j * 7 + 3] = c.x;
      floatParticleData[j * 7 + 4] = c.y;
      floatParticleData[j * 7 + 5] = c.z;
      floatParticleData[j * 7 + 6] = 1.0f;
      j += 1;
    }
  }

  void setData(const std::vector<Vec3> data)
  {
    floatParticleData.resize(data.size() * 7);
    for (size_t i = 0; i < data.size(); ++i) {
      floatParticleData[i * 7 + 0] = data[i].x;
      floatParticleData[i * 7 + 1] = data[i].y;
      floatParticleData[i * 7 + 2] = data[i].z;

      Vec3 c = color == RAINBOW_COLOR ? ColorConversion::hsvToRgb(Vec3{ float(i) / data.size() * 360.0f,1.0f,1.0f }) : computeColor(color);

      floatParticleData[i * 7 + 3] = c.x;
      floatParticleData[i * 7 + 4] = c.y;
      floatParticleData[i * 7 + 5] = c.z;
      floatParticleData[i * 7 + 6] = 1.0f;
    }
  }

  virtual std::vector<float> getData() const { return floatParticleData; }
  virtual size_t getParticleCount() const { return floatParticleData.size() / 7; }
  private:
  Vec3 color;
  std::vector<float> floatParticleData;

};

class MyGLApp : public GLApp {
  public:

  MyGLApp()
  : GLApp(800,600,4,"Octree-Demo (Dendrite Growth)"),
  simplePS(fixedParticles, 1)
  {
  }

  virtual void init() override {
#ifdef showOctree
    std::vector<float> empty;
    vbOctreeFacePos.setData(empty, 7, GL_DYNAMIC_DRAW);
    vbOctreeLinePos.setData(empty, 7, GL_DYNAMIC_DRAW);

    octreeLineArray.bind();
    octreeLineArray.connectVertexAttrib(vbOctreeLinePos, prog, "vPos", 3);
    octreeLineArray.connectVertexAttrib(vbOctreeLinePos, prog, "vColor", 4, 3);

    octreeFaceArray.bind();
    octreeFaceArray.connectVertexAttrib(vbOctreeFacePos, prog, "vPos", 3);
    octreeFaceArray.connectVertexAttrib(vbOctreeFacePos, prog, "vColor", 4, 3);
#endif

    simplePS.setColor(RAINBOW_COLOR);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef only2D
    lookFromVec = Vec3{ 0,0,cameraDistance };
#else
    lookFromVec = Vec3{ 0,cameraDistance,cameraDistance };
    if (lookFromVec.length() > 1.0f)
      lookFromVec = Vec3::normalize(lookFromVec);
#endif

    GLEnv::checkGLError("init");
  }

  virtual void animate(double animationTime) override {
    angle = animationTime;

    if (particleCount < maxParticleCount) {
      Vec3 particle = simulateOneParticle();
      particleCount++;
      fixedParticles.push_back(particle);
      octree.add(particle);
    }


  }

  virtual void draw() override {
    if (fixedParticles.size() < maxParticleCount) {
      simulationMutex.lock();
#ifdef showOctree
      octreeLineArray.bind();
      std::vector<float> data{ octree.toLineList() };
      lineVertexCount = data.size() / 7;
      vbOctreeLinePos.setData(data, 7, GL_DYNAMIC_DRAW);

      octreeFaceArray.bind();
      data = octree.toTriList();
      trisVertexCount = data.size() / 7;
      vbOctreeFacePos.setData(data, 7, GL_DYNAMIC_DRAW);
#endif
      simplePS.setData(fixedParticles);
      simulationMutex.unlock();
    }

    const Dimensions dim = glEnv.getWindowSize();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef only2D
    const Mat4 p{ Mat4::perspective(6.0f, dim.aspect(), 0.0001f, 1000.0f) };
    const Mat4 m{};
    simplePS.setPointSize(dim.height / 80.0f);
#else
    const Mat4 p{ Mat4::perspective(6.0f, dim.aspect(), 0.0001f, 1000.0f) };
    const Mat4 m{ Mat4::rotationY(25 * float(angle) / 2.0f) * Mat4::rotationX(10 * float(angle) / 2.0f) };
    simplePS.setPointSize(dim.height / 80.0f);
#endif
    const Mat4 v{ Mat4::lookAt(lookFromVec,lookAtVec,upVec) };

    simplePS.render(v * m, p);

#ifdef showOctree
    if (fixedParticles.size() < maxParticleCount) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glBlendEquation(GL_FUNC_ADD);
      glDisable(GL_CULL_FACE);
      glDepthMask(GL_FALSE);

      octreeFaceArray.bind();
      prog.enable();
      prog.setUniform(mvpLocation, p * v * m);
      glDrawArrays(GL_TRIANGLES, 0, GLsizei(trisVertexCount));

      octreeLineArray.bind();
      glDrawArrays(GL_LINES, 0, GLsizei(lineVertexCount));


      glEnable(GL_CULL_FACE);

      glDisable(GL_BLEND);
      glEnable(GL_CULL_FACE);
      glDepthMask(GL_TRUE);
    }
#endif
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override
  {
    if (action == GLENV_PRESS) {
      switch (key) {
        case GLENV_KEY_ESCAPE:
          glEnv.setClose();
          break;
        case GLENV_KEY_R:
          setAnimation(!getAnimation());
          break;
      }
    }
  }

  // returns distance from pos to nearest particle
  float mindDist(const Vec3& pos)
  {
    return octree.minDist(pos);
  }

  // returns true if the minimum distance from pos to the spawned particles is smaller than the colDist
  bool checkCollision(const Vec3& pos)
  {
    return mindDist(pos) < colDist;
  }

  Vec3 randomWalk(const Vec3& pos)
  {
#ifdef only2D
    return pos + Vec3::randomPointInDisc() * mindDist(pos);
#else
    return pos + Vec3::randomPointInSphere() * mindDist(pos);
#endif
  }

  Vec3 genRandomStartpoint()
  {
#ifdef only2D
    Vec3 current{ randomGen.rand11(),randomGen.rand11(),0 };
#else
    Vec3 current{ randomGen.rand11(),randomGen.rand11(),randomGen.rand11() };
#endif
    while (checkCollision(current))
      current = genRandomStartpoint();
    return current;
  }

  void initParticles()
  {
    fixedParticles.clear();
    fixedParticles.push_back(Vec3(0.0f, 0.0f, 0.0f));
  }

  Vec3 simulateOneParticle() {
    Vec3 current = genRandomStartpoint();
    while (!checkCollision(current)) {
      current = randomWalk(current);
      if (current.sqlength() > 5 * 5) {
        current = genRandomStartpoint();
      }
    }
    return current;
  }


  private:
  std::vector<Vec3> fixedParticles{};
  const float radius = 0.0002f;
  const float colDist = 2 * radius;
  const size_t maxParticleCount = 20000;
  size_t particleCount = 0;
  Octree octree{ 0.04f, Vec3{0.0f,0.0f,0.0f}, 5, 8 };
  float cameraDistance = 0.42f;
  bool bTerminateSimulation{ false };
  float simulationSpeed = 100.0f;
  std::mutex simulationMutex;
  Random randomGen;
  Vec3 lookFromVec;
  Vec3 lookAtVec{ 0,0,0 };
  Vec3 upVec{ 0,1,0 };
  SimpleStaticParticleSystem simplePS;
  double angle;
#ifdef showOctree

#ifdef __EMSCRIPTEN__
  std::string vsString{R"(#version 300 es
  uniform mat4 MVP;
  in vec3 vPos;
  in vec4 vColor;
  out vec4 color;
  void main() {
      gl_Position = MVP * vec4(vPos, 1.0);
      color = vColor;
  })"};

  std::string fsString{R"(#version 300 es
  precision mediump float;
  in vec4 color;
  out vec4 FragColor;
  void main() {
    FragColor = color;
  })"};
#else
  std::string vsString{R"(#version 410
  uniform mat4 MVP;
  layout (location = 0) in vec3 vPos;
  layout (location = 1) in vec4 vColor;
  out vec4 color;
  void main() {
      gl_Position = MVP * vec4(vPos, 1.0);
      color = vColor;
  })"};

  std::string fsString{R"(#version 410
  in vec4 color;
  out vec4 FragColor;
  void main() {
    FragColor = color;
  })"};

#endif

  GLProgram prog{ GLProgram::createFromString(vsString, fsString) };
  GLint mvpLocation{ prog.getUniformLocation("MVP") };

  GLArray octreeLineArray{};
  GLBuffer vbOctreeLinePos{ GL_ARRAY_BUFFER };
  GLArray octreeFaceArray{};
  GLBuffer vbOctreeFacePos{ GL_ARRAY_BUFFER };

  size_t trisVertexCount = 0;
  size_t lineVertexCount = 0;
#endif

};

int main(int argc, char** argv) {
  MyGLApp myApp;
  myApp.run();
  return EXIT_SUCCESS;
}
