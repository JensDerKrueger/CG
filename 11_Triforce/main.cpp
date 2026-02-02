#include <fstream>

#include <GLApp.h>

class MyGLApp : public GLApp {
public:
  Mat4 modelView{};
  Mat4 projection{};
  GLuint program{0};
  GLint modelViewMatrixUniform{-1};
  GLint projectionMatrixUniform{-1};
  GLuint vbos{0};
  GLuint vaos{0};
  
  const GLfloat vertexPositions[9] = {
     1.5f, 2.0f, 0.0f,
    -1.5f, 0.0f, 0.0f,
     1.5f, 0.0f, 0.0f
  };
  
  MyGLApp()
    : GLApp(800,600,4,"Assignment 11 - Triforce")
  {}
  
  virtual void init() override {
    setupShaders();
    setupGeometry();
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
  }

  virtual void animate(double animationTime) override {    
  }

  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    
    GL(glUseProgram(program));
    modelView = Mat4::translation(0.0f, -1.0f, 0.0f);
    GL(glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_TRUE, modelView));
    
    GL(glBindVertexArray(vaos));
    GL(glDrawArrays(GL_TRIANGLES, 0, sizeof(vertexPositions) / sizeof(vertexPositions[0]) / 3));
    GL(glBindVertexArray(0));
    GL(glUseProgram(0));
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override{
    GLApp::resize(winDim, fbDim);

    const float ratio = fbDim.aspect();

    projection = Mat4::ortho(-ratio * 1.5f, ratio * 1.5f, -1.5f, 1.5f, -10.0f, 10.0f);
    GL(glUseProgram(program));
    GL(glUniformMatrix4fv(projectionMatrixUniform, 1, GL_TRUE, projection));
    GL(glUseProgram(0));
  }

  std::string loadFile(const std::string& filename) {
    std::ifstream shaderFile{ filename };
    if (!shaderFile)
    {
      throw GLException{ std::string("Unable to open file ") + filename };
    }
    std::string str;
    std::string fileContents;
    while (std::getline(shaderFile, str)) {
      fileContents += str + "\n";
    }
    return fileContents;
  }
  
  GLuint createShaderFromFile(GLenum type, const std::string& sourcePath) {
    const std::string shaderCode = loadFile(sourcePath);
    const GLchar* c_shaderCode = shaderCode.c_str();
    const GLuint s = glCreateShader(type);
    GL(glShaderSource(s, 1, &c_shaderCode, NULL));
    glCompileShader(s); checkAndThrowShader(s);
    return s;
  }
  
  void setupShaders() {
    const std::string vertexSrcPath = "res/shaders/vertexShader.vert";
    const std::string fragmentSrcPath = "res/shaders/fragmentShader.frag";
    GLuint vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    GLuint fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    
    program = glCreateProgram();
    GL(glAttachShader(program, vertexShader));
    GL(glAttachShader(program, fragmentShader));
    GL(glLinkProgram(program));
    checkAndThrowProgram(program);
    
    GL(glUseProgram(program));
    modelViewMatrixUniform = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
    GL(glUseProgram(0));
  }
  
  void setupGeometry() {
    const GLint vertexPos = glGetAttribLocation(program, "vertexPosition");
    
    GL(glGenVertexArrays(1, &vaos));
    GL(glBindVertexArray(vaos));
    
    GL(glGenBuffers(1, &vbos));
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions,
                    GL_STATIC_DRAW));
    
    GL(glEnableVertexAttribArray(vertexPos));
    GL(glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    GL(glBindVertexArray(0););
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override
  {
  }
};

int main(int argc, char** argv) {
  MyGLApp myApp;
  myApp.run();
  return EXIT_SUCCESS;
}

