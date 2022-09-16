#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <string>
#include <iostream>

#ifdef _WIN32
#define ASSERT(x) if(!(x)) __debugbreak()
#else
#define ASSERT(x) x
#endif

#ifndef NDEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define GLCheckError() ASSERT(GLLogError(__FILE__, __LINE__))
#else
#define GLCall(x)
#define GLCheckError()
#endif

std::string errorString(GLenum error);
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
bool GLLogError(const char* file, int line);

class GLException : public std::exception
{
private:
	std::string whatStr;

public:
	GLException(const std::string& whatStr)
		: whatStr(whatStr)
	{
	}

	virtual const char* what() const throw()
	{
		return whatStr.c_str();
	}
};