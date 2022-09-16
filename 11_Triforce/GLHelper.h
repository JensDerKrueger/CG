#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLDebug.h>
#include <GLEnv.h>
#include <vector>
#include <fstream>
#include <exception>
#include <string>

namespace Shader
{
	GLuint createShaderFromFile(GLenum type, const std::string& sourcePath);
	void checkAndThrowProgram(GLuint program);
};
