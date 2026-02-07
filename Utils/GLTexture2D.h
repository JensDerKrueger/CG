#pragma once

#include <vector>

#include "GLEnv.h"
#include "Image.h"

class GLTexture2D {
public:
	GLTexture2D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);
	~GLTexture2D();
	
  GLTexture2D(const Image& image,
              GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);
  GLTexture2D(const GLTexture2D& other);
  GLTexture2D& operator=(const GLTexture2D& other);
    
	const GLuint getId() const;
  void clear();
  void setEmpty(uint32_t width, uint32_t height, uint8_t componentCount, GLDataType dataType=GLDataType::BYTE);
  void setData(const Image& image);
  void setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);
  void setData(const std::vector<GLubyte>& data);
  void setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);
  void setData(const std::vector<GLfloat>& data);
  void setData(const std::vector<GLhalf>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);
  void setData(const std::vector<GLhalf>& data);
  void setFilter(GLint magFilter, GLint minFilter);
  
  void setPixel(const std::vector<GLubyte>& data, uint32_t x, uint32_t y);
  
  void generateMipmap();
  
  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*componentCount;}
  GLDataType getType() const {return dataType;}

#ifndef __EMSCRIPTEN__
  Image getImage();
  const std::vector<GLubyte>& getDataByte();
  const std::vector<GLhalf>& getDataHalf();
  const std::vector<GLfloat>& getDataFloat();
#endif

private:
	GLuint id;
  GLint internalformat;
	GLenum format;
	GLenum type;

  GLint magFilter;
  GLint minFilter;
  GLint wrapX;
  GLint wrapY;
  std::vector<GLubyte> data;
  std::vector<GLhalf> hdata;
  std::vector<GLfloat> fdata;
  uint32_t width;
  uint32_t height;
  uint8_t componentCount;
  GLDataType dataType;
  
  void setData(GLvoid* data, uint32_t width, uint32_t height, 
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
