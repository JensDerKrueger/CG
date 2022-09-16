#include <GLApp.h>
#include "GLHelper.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif // __APPLE__

class MyGLApp : public GLApp
{
public:
  double time{};
  
  Mat4 modelView = Mat4();
  Mat4 projection = Mat4();
  GLuint program{};
  GLint modelViewMatrixUniform = -1;
  GLint projectionMatrixUniform = -1;
  GLuint vbos[1]{ };
  GLuint vaos[1]{ };
  
  const GLfloat vertexPositions[9] = {
    1.5f, 2.0f, 0.0f,
    -1.5f, 0.0f, 0.0f,
    1.5f, 0.0f, 0.0f
  };
  
  MyGLApp()
    : GLApp(800,600,4,"Assignment 11 - Triforce")
  {}
  
  virtual void init() override
  {
    const GLubyte* glVersion = glGetString(GL_VERSION);
    const GLubyte* slVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << glVersion << std::endl;
    std::cout << slVersion << std::endl;
    
    time = glfwGetTime();
    setupShaders();
    setupGeometry();
  }
  
  virtual void draw() override
  {
    double t = glfwGetTime();
    double d = t - time;
    time = t;
    
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL(glClear(GL_COLOR_BUFFER_BIT));
    
    GL(glUseProgram(program));
    modelView = Mat4::translation(0.0f, -1.0f, 0.0f);
    GL(glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_TRUE, modelView));
    
    GL(glBindVertexArray(vaos[0]));
    GL(glDrawArrays(GL_TRIANGLES, 0, sizeof(vertexPositions) / sizeof(vertexPositions[0]) / 3));
    GL(glBindVertexArray(0));
    GL(glUseProgram(0));
  }
  
  virtual void resize(int width, int height) override
  {
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    std::cout << "reshaping window, offset: (0 0) resolution: " << width << "x"
    << height << " ratio: " << ratio << std::endl;
    
    projection = Mat4::ortho(-ratio * 1.5f, ratio * 1.5f, -1.5f, 1.5f, -10.0f, 10.0f);
    GL(glUseProgram(program));
    GL(glUniformMatrix4fv(projectionMatrixUniform, 1, GL_TRUE, projection));
    GL(glUseProgram(0));
    glViewport(0, 0, width, height);
  }
  
  void setupShaders()
  {
    std::string pathToExe = "";
#ifdef __APPLE__
    char path[1024];
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
    pathToExe = std::string(path);
    while (pathToExe.size() > 0 && pathToExe.back() != '/') pathToExe.pop_back();
#endif
    std::string vertexSrcPath = pathToExe + "res/shaders/vertexShader.vert";
    std::string fragmentSrcPath = pathToExe + "res/shaders/fragmentShader.frag";
    GLuint vertexShader = Shader::createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    GLuint fragmentShader = Shader::createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    
    program = glCreateProgram();
    GL(glAttachShader(program, vertexShader));
    GL(glAttachShader(program, fragmentShader));
    glLinkProgram(program);
    Shader::checkAndThrowProgram(program);
    
    glUseProgram(program);
    modelViewMatrixUniform = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
    GL(glUseProgram(0));
  }
  
  void setupGeometry()
  {
    GLint vertexPos = glGetAttribLocation(program, "vertexPosition");
    
    GL(glGenVertexArrays(1, vaos));
    GL(glBindVertexArray(vaos[0]));
    
    GL(glGenBuffers(1, vbos));
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos[0]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW));
    
    GL(glEnableVertexAttribArray(vertexPos));
    GL(glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    GL(glBindVertexArray(0););
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override
  {
    
  }
};

int main(int argc, char** argv)
{
  MyGLApp myApp;
  myApp.run();
  return EXIT_SUCCESS;
}

