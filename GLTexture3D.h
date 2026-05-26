#pragma once

#include <vector>

#include "GLEnv.h"
#include "half.h"

class GLTexture3D {
public:
	GLTexture3D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT,
              GLint wrapZ=GL_REPEAT, GLDataType dataType=GLDataType::BYTE);
	~GLTexture3D();
	
  GLTexture3D(const GLTexture3D& other);
  GLTexture3D& operator=(GLTexture3D other);
    
	const GLuint getId() const;
  void clear();
  void setFilter(GLint magFilter, GLint minFilter);
  void setEmpty(uint32_t width, uint32_t height, uint32_t depth,
                uint8_t componentCount, GLDataType dataType=GLDataType::BYTE);
	void setData(const std::vector<GLubyte>& data, uint32_t width,
               uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<GLubyte>& data);
#ifndef __EMSCRIPTEN__
  void setData(const std::vector<GLushort>& data, uint32_t width,
               uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<GLushort>& data);
#endif
  void setData(const std::vector<half::Half>& data, uint32_t width,
               uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<half::Half>& data);
  void setData(const std::vector<GLfloat>& data, uint32_t width,
               uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<GLfloat>& data);

  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getDepth() const {return depth;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*depth*componentCount;}
  GLDataType getDataType() const {return dataType;}

  static uint32_t getMaxSize() {
    GLint max3DSize = 0;
    GL(glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DSize));
    return uint32_t(max3DSize);
  }

  void setData(const GLvoid* data, uint32_t width, uint32_t height,
               uint32_t depth, uint8_t componentCount, GLDataType dataType);

private:
	GLuint id;

  GLint magFilter;
  GLint minFilter;
  GLint wrapX;
  GLint wrapY;
  GLint wrapZ;
  std::vector<GLubyte> data;
#ifndef __EMSCRIPTEN__
  std::vector<GLushort> sdata;
#endif
  std::vector<half::Half> hdata;
  std::vector<GLfloat> fdata;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint8_t componentCount;
  GLDataType dataType;
  
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
