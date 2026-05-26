#pragma once

#include <vector>

#include "GLEnv.h"
#include "half.h"

class GLTexture1D {
public:
	GLTexture1D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLDataType dataType=GLDataType::BYTE);

  GLTexture1D(const GLTexture1D& other);    
  GLTexture1D& operator=(GLTexture1D other);    
  
	~GLTexture1D();

  void clear();
  void setEmpty(uint32_t size, uint8_t componentCount,
                GLDataType dataType=GLDataType::BYTE);

	const GLuint getId() const;
	void setData(const std::vector<GLubyte>& data, uint32_t size, 
               uint8_t componentCount=4);
  void setData(const std::vector<GLubyte>& data);
#ifndef __EMSCRIPTEN__
  void setData(const std::vector<GLushort>& data, uint32_t size,
               uint8_t componentCount=4);
  void setData(const std::vector<GLushort>& data);
#endif
  void setData(const std::vector<half::Half>& data, uint32_t size,
               uint8_t componentCount=4);
  void setData(const std::vector<half::Half>& data);
  void setData(const std::vector<GLfloat>& data, uint32_t size,
               uint8_t componentCount=4);
  void setData(const std::vector<GLfloat>& data);

private:
	GLuint id;

  GLDataType dataType;
  GLint magFilter;
  GLint minFilter;
  GLint wrapX;
  std::vector<GLubyte> data;
#ifndef __EMSCRIPTEN__
  std::vector<GLushort> sdata;
#endif
  std::vector<half::Half> hdata;
  std::vector<GLfloat> fdata;
  uint32_t size;
  uint8_t componentCount;

  void setData(GLvoid* data, uint32_t size,
               uint8_t componentCount, GLDataType dataType);
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
