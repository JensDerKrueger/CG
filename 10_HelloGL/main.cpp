#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

#include <GLDebug.h>

GLFWwindow* window;
GLuint vbo;
GLuint vao;
GLuint program;

// triangle vertex position data
constexpr float triangle[] = {
	 0.0f,  0.5f, 0.0f, // top
	-0.5f, -0.5f, 0.0f,	// bottom left
	 0.5f, -0.5f, 0.0f  // bottom right
};

// vertex shader code
const GLchar* vertexShaderSource{
  "#version 410 core\n"
  "layout(location = 0) in vec3 vPos;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(vPos, 1.0);\n"
  "}\0"};

// fragment shader code
const GLchar* fragmentShaderSource{
  "#version 410 core\n"
  "out vec4 fragColor;\n"
  "void main()\n"
  "{\n"
  "  fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);\n"
  "}\0"};

void init() {
  const GLubyte* glVersion = glGetString(GL_VERSION);
  const GLubyte* slVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  std::cout << glVersion << std::endl;
  std::cout << slVersion << std::endl;
}

void draw() {
  GL( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );
  GL( glClear(GL_COLOR_BUFFER_BIT) );
  
  GL( glBindVertexArray(vao) );
  GL( glUseProgram(program) );
  GL( glDrawArrays(GL_TRIANGLES, 0, 3) );
  GL( glBindVertexArray(0) );
  
  glfwSwapBuffers(window);
}

void setupShaders() {
  // create the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GL( glShaderSource(vertexShader, 1, &vertexShaderSource, NULL) );
  GL( glCompileShader(vertexShader) );
  checkAndThrowShader(vertexShader);
  
  // create the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  GL( glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL) );
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

void setupGeometry() {
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

void checkProgramLinkStatus(GLuint programId) {
  GLint success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(programId, 512, NULL, infoLog);
    std::cout << "Error: Program link failed.\n" << infoLog << std::endl;
  }
}

void checkShaderCompileStatus(GLuint shaderId) {
  GLint success;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
    std::cout << "Error: Shader compilation failed.\n" << infoLog << std::endl;
  }
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  GL( glViewport(0, 0, w, h) );
}

int main(int argc, char** argv) {
	// initialize glfw and glew
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "Assignment 10 - Hello OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	const GLenum err(glewInit());
	if (err != GLEW_OK) {
		std::stringstream s;
		s << "Failed to init GLEW " << glewGetErrorString(err) << std::endl;
		glfwTerminate();
		throw GLException(s.str());
	}

	// set the opengl viewport to match the window resolution
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);

	init();
	setupShaders();
	setupGeometry();

	// main loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		draw();
		glfwPollEvents();
	}

  glfwTerminate();
	return EXIT_SUCCESS;
}

