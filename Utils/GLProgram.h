#pragma once

#include <vector>
#include <string>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "GLTexture1D.h"
#include "GLTexture2D.h"
#include "GLTexture3D.h"
#include "GLDepthTexture.h"
#include "GLTextureCube.h"

class ProgramException : public std::exception {
	public:
		ProgramException(const std::string& whatStr) : whatStr(whatStr) {}
		virtual const char* what() const throw() {
			return whatStr.c_str();
		}
	private:
		std::string whatStr;
};

class GLProgram {
public:
	~GLProgram();

	static GLProgram createFromFiles(const std::vector<std::string>& vs, const std::vector<std::string>& fs,
                                   const std::vector<std::string>& gs = std::vector<std::string>(),
                                   bool quietFail=false, bool addVersionHeader = false);
	static GLProgram createFromStrings(const std::vector<std::string>& vs, const std::vector<std::string>& fs,
                                     const std::vector<std::string>& gs = std::vector<std::string>(),
                                     bool quietFail=false, bool addVersionHeader = false);

	static GLProgram createFromFile(const std::string& vs, const std::string& fs,
                                  const std::string& gs="", bool quietFail=false,
                                  bool addVersionHeader = false);
	static GLProgram createFromString(const std::string& vs, const std::string& fs,
                                    const std::string& gs="", bool quietFail=false,
                                    bool addVersionHeader = false);

  static const std::string& getShaderPreamble();
  static void setShaderPreamble(const std::string& preamble);

  GLProgram(const GLProgram& other);
  GLProgram& operator=(const GLProgram& other);
  
  GLint getAttributeLocation(const std::string& id) const;
  GLint getUniformLocation(const std::string& id) const;

  void setUniform(const std::string& id, float value) const;
  void setUniform(const std::string& id, const Vec2& value) const;
  void setUniform(const std::string& id, const Vec3& value) const;
  void setUniform(const std::string& id, const Vec4& value) const;
  void setUniform(const std::string& id, const std::vector<float>& value) const;
  void setUniform(const std::string& id, const std::vector<Vec2>& value) const;
  void setUniform(const std::string& id, const std::vector<Vec3>& value) const;
  void setUniform(const std::string& id, const std::vector<Vec4>& value) const;
  void setUniform(const std::string& id, int value) const;
  void setUniform(const std::string& id, const Vec2i& value) const;
  void setUniform(const std::string& id, const Vec3i& value) const;
  void setUniform(const std::string& id, const Vec4i& value) const;
  void setUniform(const std::string& id, const Mat4& value, bool transpose=false) const;
  
  void setTexture(const std::string& id, const GLTexture1D& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTexture2D& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTexture3D& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLDepthTexture& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTextureCube& texture, GLenum unit=0) const;


	void setUniform(GLint id, float value) const;
  void setUniform(GLint id, const Vec2& value) const;
	void setUniform(GLint id, const Vec3& value) const;
  void setUniform(GLint id, const Vec4& value) const;

  void setUniform(GLint id, int value) const;
  void setUniform(GLint id, const Vec2i& value) const;
  void setUniform(GLint id, const Vec3i& value) const;
  void setUniform(GLint id, const Vec4i& value) const;
	void setUniform(GLint id, const Mat4& value, bool transpose=false) const;

  void setUniform(GLint id, const std::vector<float>& value) const;
  void setUniform(GLint id, const std::vector<Vec2>& value) const;
  void setUniform(GLint id, const std::vector<Vec3>& value) const;
  void setUniform(GLint id, const std::vector<Vec4>& value) const;
  void setUniform(GLint id, const std::vector<int>& value) const;
  void setUniform(GLint id, const std::vector<Vec2i>& value) const;
  void setUniform(GLint id, const std::vector<Vec3i>& value) const;
  void setUniform(GLint id, const std::vector<Vec4i>& value) const;
  void setUniform(GLint id, const std::vector<Mat4>& value, bool transpose=false) const;
  
  void setTexture(GLint id, const GLTexture1D& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLTexture2D& texture, GLenum unit=0) const;
	void setTexture(GLint id, const GLTexture3D& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLTextureCube& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLDepthTexture& texture, GLenum unit=0) const;


  void unsetTexture1D(GLenum unit) const;
  void unsetTexture2D(GLenum unit) const;
  void unsetTexture3D(GLenum unit) const;

	void enable() const;
	void disable() const;

private:
  bool quietFail;
  bool addVersionHeader;
	GLuint glVertexShader;
	GLuint glFragmentShader;
	GLuint glGeometryShader;
	GLuint glProgram;

  static std::string shaderPreamble;

  std::vector<std::string> vertexShaderStrings;
  std::vector<std::string> fragmentShaderStrings;
  std::vector<std::string> geometryShaderStrings;
	
	static std::string loadFile(const std::string& filename);
	
	static GLuint createShader(GLenum type, const GLchar** src, GLsizei count);

  GLProgram(std::vector<std::string> vertexShaderStrings,
            std::vector<std::string> fragmentShaderStrings,
            std::vector<std::string> geometryShaderStrings,
            bool quietFail,
            bool addVersionHeader);

  void programFromVectors(std::vector<std::string> vs, std::vector<std::string> fs, std::vector<std::string> gs);
};


/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
