#include <filesystem>
#include <sstream>

#include <GLApp.h>
#include <OBJFile.h>
#include <Mat4.h>
#include <FontRenderer.h>

class MyGLApp : public GLApp {
public:
  double angle{0.0};
  std::vector<float> data;

  FontRenderer fr{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fe{nullptr};

  const std::string vertexshaderName = "shader.vert";
  const std::string fragmentshaderName = "shader.frag";

  GLProgram myProgram;
  GLArray myArray;
  GLBuffer myBuffer;
  Mat4 M,V,P;
  std::string text;
  Vec4 textColor{};


  MyGLApp() :
    GLApp{640,480,1,"Shader Editor"},
    myProgram(GLProgram::createFromFile(vertexshaderName, fragmentshaderName)),
    myBuffer{GL_ARRAY_BUFFER}
  {}

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glClearColor(0,0,0,0));
    fe = fr.generateFontEngine();

    data = loadOBJ("bunny.obj");
    setupShaders();
  }

  void setupShaders() {
    static std::filesystem::file_time_type lastVertTime;
    static std::filesystem::file_time_type lastFragTime;
    const auto vertTime = std::filesystem::last_write_time(vertexshaderName);
    const auto fragTime = std::filesystem::last_write_time(fragmentshaderName);

    if (vertTime == lastVertTime && fragTime == lastFragTime) return;
    lastVertTime = vertTime;
    lastFragTime = fragTime;

    try {
      GLProgram dummy = GLProgram::createFromFile(vertexshaderName, fragmentshaderName);
    } catch (const GLException& e) {
      std::stringstream ss;
      ss << e.what();
      text = ss.str();
      textColor = Vec4{1,0,0,1};
      return;
    }
    text = "Shader ok";
    textColor = Vec4{1,1,1,1};

    myProgram = GLProgram::createFromFile(vertexshaderName, fragmentshaderName);

    myProgram.enable();
    myArray.bind();
    myBuffer.setData(data,10,GL_DYNAMIC_DRAW);
    myArray.connectVertexAttrib(myBuffer, myProgram, "vPos", 3);
    myArray.connectVertexAttrib(myBuffer, myProgram, "vColor", 4, 3);
    myArray.connectVertexAttrib(myBuffer, myProgram, "vNormal", 3, 7);
  }

  virtual void animate(double animationTime) override{
    setupShaders();

    angle = animationTime*30;

    P = Mat4::perspective(45, glEnv.getFramebufferSize().aspect(), 0.0001f, 100);
    V = Mat4::lookAt({0,0,2},{0,0,0},{0,1,0}) * Mat4::rotationY(float(angle));
    M = Mat4{};

    const Mat4 MVP  = P*V*M;
    const Mat4 MV   = V*M;
    const Mat4 MVit = Mat4::transpose(Mat4::inverse(MV));

    myProgram.enable();
    try {
      myProgram.setUniform("MVP", MVP);
    } catch (...) {}
    try {
      myProgram.setUniform("MV", MV);
    } catch (...) {}
    try {
      myProgram.setUniform("MVit", MVit);
    } catch (...) {}
  }
  
  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));

    myProgram.enable();
    myArray.bind();
    const GLsizei vertexCount = GLsizei(data.size()/10);
    GL(glDrawArrays(GL_TRIANGLES, 0, vertexCount));

    drawText();
  }

  void drawText() {
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
    const Dimensions dim{ glEnv.getFramebufferSize() };
    fe->render(text, dim.aspect(), 0.03f, {0,-0.9f}, Alignment::Center, textColor);
    GL(glDisable(GL_BLEND));
  }

private:
  std::vector<float> loadOBJ(const std::string& filename) {
    std::vector<float> target;
    const OBJFile m{filename, true};
    for (const OBJFile::IndexType& triangle : m.indices) {
      for (const size_t& index : triangle) {
        target.push_back(m.vertices[index][0]);
        target.push_back(m.vertices[index][1]);
        target.push_back(m.vertices[index][2]);
        
        target.push_back(m.vertices[index][0]+0.5f);
        target.push_back(m.vertices[index][1]+0.5f);
        target.push_back(m.vertices[index][2]+0.5f);
        target.push_back(1.0f);
        
        target.push_back(m.normals[index][0]);
        target.push_back(m.normals[index][1]);
        target.push_back(m.normals[index][2]);
      }
    }
    return target;
  }

} myApp;

int main(int argc, char ** argv) {
  myApp.run();
  return EXIT_SUCCESS;
}  
