#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

#include "GLDebug.h"

GLFWwindow* window;
GLuint vbo;
GLuint vao;
GLuint program;

// triangle vertex position data
const float triangle[] = {
	 0.0f,  0.5f, 0.0f, // top
	-0.5f, -0.5f, 0.0f,	// bottom left
	 0.5f, -0.5f, 0.0f  // bottom right
};

// vertex shader code
const char* vertexShaderSource =
"#version 410 core\n"
"layout(location = 0) in vec3 vPos;\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(vPos, 1.0);\n"
"}\0";

// fragment shader code
const char* fragmentShaderSource =
"#version 410 core\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"  fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);\n"
"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void checkShaderCompileStatus(GLuint shaderId);
void checkProgramLinkStatus(GLuint programId);
void init();
void draw();
void setupShaders();
void setupGeometry();

int main(int argc, char** argv)
{
	// initialize glfw and glew
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "Assignment 01 - Hello OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum err(glewInit());
	if (err != GLEW_OK)
	{
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
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		draw();

		glfwPollEvents();
	}


	glfwTerminate();
	return EXIT_SUCCESS;
}


void init()
{
	const GLubyte* glVersion = glGetString(GL_VERSION); GLCheckError();
	const GLubyte* slVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); GLCheckError();
	std::cout << glVersion << std::endl;
	std::cout << slVersion << std::endl;
}

void draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao); GLCheckError();
	glUseProgram(program); GLCheckError();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0); GLCheckError();

	glfwSwapBuffers(window);
}

void setupShaders()
{
	GLuint vertexShader;
	GLuint fragmentShader;

	// create the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	checkShaderCompileStatus(vertexShader);

	// create the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompileStatus(fragmentShader);

	// link shaders into program
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	checkProgramLinkStatus(program);

	glDeleteShader(vertexShader); GLCheckError();
	glDeleteShader(fragmentShader); GLCheckError();
}

void setupGeometry()
{
	// define VAO for triangle
	glGenVertexArrays(1, &vao); GLCheckError();
	glBindVertexArray(vao); GLCheckError();

	// upload vertex positions to VBO
	glGenBuffers(1, &vbo); GLCheckError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GLCheckError();
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW); GLCheckError();

	glEnableVertexAttribArray(0); GLCheckError();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); GLCheckError();

	glBindVertexArray(0); GLCheckError();
}

void checkProgramLinkStatus(GLuint programId)
{
	GLint success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "Error: Program link failed.\n" << infoLog << std::endl;
	}
}

void checkShaderCompileStatus(GLuint shaderId)
{
	GLint success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "Error: Shader compilation failed.\n" << infoLog << std::endl;
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h); GLCheckError();
}
