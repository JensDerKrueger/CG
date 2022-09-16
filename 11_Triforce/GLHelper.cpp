#include "GLHelper.h"

namespace Shader
{
	void checkAndThrowShader(GLuint shader)
	{
		GLint success[1] = { GL_TRUE };
		glGetShaderiv(shader, GL_COMPILE_STATUS, success);
		if (success[0] == GL_FALSE)
		{
			GLint log_length{ 0 };
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
			log_length = std::min(static_cast<GLint>(4096), log_length);
			std::vector<GLchar> log(log_length);
			glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), NULL, log.data());
			std::string str{ log.data() };
			throw GLException{ str };
		}
	}

	GLuint createShader(GLenum type, const GLchar** src, GLsizei count) {
		if (count == 0) return 0;
		GLuint s = glCreateShader(type);
		GL(glShaderSource(s, count, src, NULL));
		glCompileShader(s); checkAndThrowShader(s);
		return s;
	}


	std::string loadFile(const std::string& filename)
	{
		std::ifstream shaderFile{ filename };
		if (!shaderFile)
		{
			throw GLException{ std::string("Unable to open file ") + filename };
		}
		std::string str;
		std::string fileContents;
		while (std::getline(shaderFile, str))
		{
			fileContents += str + "\n";
		}
		return fileContents;
	}

	GLuint createFromFile(GLenum type, const std::string& sourcePath)
	{
		std::string shaderCode = loadFile(sourcePath);
		const GLchar* c_shaderCode = shaderCode.c_str();
		return createShader(type, &c_shaderCode, 1);
	}


	GLuint createShaderFromFile(GLenum type, const std::string& sourcePath)
	{
		return createFromFile(type, std::string{ sourcePath });
	}

	void checkAndThrowProgram(GLuint program)
	{
		GLint linked{ GL_TRUE };
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE)
		{
			GLint log_length{ 0 };
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
			log_length = std::min(static_cast<GLint>(4096), log_length);
			std::vector<GLchar> log(log_length);
			glGetProgramInfoLog(program, static_cast<GLsizei>(log.size()), NULL, log.data());
			std::string str{ log.data() };
			throw GLException{ str };
		}
	}


}
