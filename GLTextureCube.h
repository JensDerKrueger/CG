#pragma once

#include <vector>

#include "GLEnv.h"
#include "Image.h"

enum class Face {
  POSX=0,
  NEGX,
  POSY,
  NEGY,
  POSZ,
  NEGZ
};

class GLTextureCube {
public:
	GLTextureCube(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
                GLint wrapX=GL_CLAMP_TO_EDGE, GLint wrapY=GL_CLAMP_TO_EDGE,
                GLint wrapZ=GL_CLAMP_TO_EDGE);
	~GLTextureCube();
	
  GLTextureCube(const Image& image0, const Image& image1, const Image& image2,
                const Image& image3, const Image& image4, const Image& image5,
                GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
                GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);
  GLTextureCube(const GLTextureCube& other);
  GLTextureCube& operator=(const GLTextureCube& other);
    
	const GLuint getId() const;
  void clear();
  void setEmpty(uint32_t width, uint32_t height, uint8_t componentCount,
                GLDataType dataType=GLDataType::BYTE);
  void setData(const Image& image, Face face);
  void setData(const std::vector<GLubyte>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);
  void setData(const std::vector<GLubyte>& data, Face face);
  void setData(const std::vector<GLfloat>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);
  void setData(const std::vector<GLfloat>& data, Face face);
  void setData(const std::vector<GLhalf>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);
  void setData(const std::vector<GLhalf>& data, Face face);
  void setFilter(GLint magFilter, GLint minFilter);

  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*componentCount;}
  GLDataType getType() const {return dataType;}

  void generateMipmap();

private:
	GLuint id;
	GLint internalformat;
	GLenum format;
	GLenum type;

  GLint magFilter;
  GLint minFilter;
  GLint wrapX;
  GLint wrapY;
  GLint wrapZ;
  std::vector<GLubyte> data;
  std::vector<GLhalf> hdata;
  std::vector<GLfloat> fdata;
  uint32_t width{0};
  uint32_t height{0};
  uint8_t componentCount{0};
  GLDataType dataType;
  
  void setData(GLvoid* data, uint32_t width, uint32_t height, Face face,
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
