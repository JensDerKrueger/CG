#include <GLEnv.h>
#include <Mat4.h>

#include <iostream>
#include <sstream>

#include <GLProgram.h>

GLuint vbo;
GLuint vao;
GLuint program;

// triangle vertex position data
constexpr float triangle[] = {
  0.0f,  0.5f, 0.0f, // top
  -0.5f, -0.5f, 0.0f,  // bottom left
  0.5f, -0.5f, 0.0f  // bottom right
};

const GLchar* vertexShaderSource{
R"(in vec3 vPos;
void main()
{
  gl_Position = vec4(vPos, 1.0);
}
)"
};

const GLchar* fragmentShaderSource{
R"(out vec4 fragColor;
void main()
{
  fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);
}
)"
};


static void draw(void* arg=nullptr) {
  GL( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );
  GL( glClear(GL_COLOR_BUFFER_BIT) );
  
  GL( glBindVertexArray(vao) );
  GL( glUseProgram(program) );
  GL( glDrawArrays(GL_TRIANGLES, 0, 3) );
  GL( glBindVertexArray(0) );
}

static void setupShaders() {
  // create the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const std::string fullSourceV = GLProgram::getShaderPreamble() + vertexShaderSource;
  const GLchar* c_shaderCodeV = fullSourceV.c_str();
  GL( glShaderSource(vertexShader, 1, &c_shaderCodeV, NULL) );
  GL( glCompileShader(vertexShader) );
  checkAndThrowShader(vertexShader);

  // create the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const std::string fullSourceF = GLProgram::getShaderPreamble() + fragmentShaderSource;
  const GLchar* c_shaderCodeF = fullSourceF.c_str();
  GL( glShaderSource(fragmentShader, 1, &c_shaderCodeF, NULL) );
  GL( glCompileShader(fragmentShader) );
  checkAndThrowShader(fragmentShader);

  // link shaders into program
  program = glCreateProgram();
  GL( glAttachShader(program, vertexShader) );
  GL( glAttachShader(program, fragmentShader) );
  GL( glLinkProgram(program) );
  checkAndThrowProgram(program);

  GL( glDeleteShader(vertexShader) );
  GL( glDeleteShader(fragmentShader) );
}

static void setupGeometry() {
  // define VAO for triangle
  GL( glGenVertexArrays(1, &vao) );
  GL( glBindVertexArray(vao) );

  // upload vertex positions to VBO
  GL( glGenBuffers(1, &vbo) );
  GL( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
  GL( glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW) );

  GL( glEnableVertexAttribArray(0) );
  GL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0) );

  GL( glBindVertexArray(0) );
}

#ifndef __EMSCRIPTEN__
static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

static void sizeCallback(GLFWwindow* window, int width, int height) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  GL( glViewport(0, 0, w, h) );
}

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    try {
        GLEnv glEnv{ 800,600,1,"My First OpenGL Program",true,false };
        glEnv.setKeyCallback(keyCallback);
        glEnv.setResizeCallback(sizeCallback);
        setupShaders();
        setupGeometry();
        while (!glEnv.shouldClose()) {
            draw();
            glEnv.endOfFrame();
        }
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

#else
int main(int argc, char** argv) {
  GLEnv glEnv{800,600,1,"My First OpenGL Program",true,false};
  setupShaders();
  setupGeometry();
  emscripten_set_main_loop_arg(draw, nullptr, 0, 1);
  while (!glEnv.shouldClose()) {
    draw();
    glEnv.endOfFrame();
  }
  return EXIT_SUCCESS;
}
#endif
