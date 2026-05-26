#include <array>
#include <sstream>

#include "GLTextureCube.h"

GLTextureCube::GLTextureCube(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY, GLint wrapZ) :
  id(0),
  magFilter(magFilter),
  minFilter(minFilter),
  wrapX(wrapX),
  wrapY(wrapY),
  wrapZ(wrapZ),
  width(0),
  height(0),
  componentCount(0),
  dataType(GLDataType::BYTE)
{
  GL(glGenTextures(1, &id));
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTextureCube::GLTextureCube(const Image& image0,
                             const Image& image1,
                             const Image& image2,
                             const Image& image3,
                             const Image& image4,
                             const Image& image5,
                             GLint magFilter, GLint minFilter,
                             GLint wrapX, GLint wrapY) :
GLTextureCube(magFilter, minFilter, wrapX, wrapY)
{
  setData(image0,Face::POSX);
  setData(image1,Face::NEGX);
  setData(image2,Face::POSY);
  setData(image3,Face::NEGY);
  setData(image4,Face::POSZ);
  setData(image5,Face::NEGZ);
}


void GLTextureCube::setFilter(GLint magFilter, GLint minFilter) {
  this->magFilter = magFilter;
  this->minFilter = minFilter;
  
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTextureCube::~GLTextureCube() {
  GL(glDeleteTextures(1, &id));
}

GLTextureCube::GLTextureCube(const GLTextureCube& other) :
  GLTextureCube(other.magFilter, other.minFilter, other.wrapX, other.wrapY, other.wrapZ)
{
  if (other.height > 0 && other.width > 0) {
    for (size_t face = 0;face<6;++face) {
      switch (other.dataType) {
        case GLDataType::BYTE  :
          setData(other.data, other.width, other.height, Face(face), other.componentCount);
          break;
#ifndef __EMSCRIPTEN__
        case GLDataType::SHORT  :
          setData(other.sdata, other.width, other.height, Face(face), other.componentCount);
          break;
#endif
        case GLDataType::HALF  :
          setData(other.hdata, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::FLOAT :
          setData(other.fdata, other.width, other.height, Face(face), other.componentCount);
          break;
      }
    }
  }
}

GLTextureCube& GLTextureCube::operator=(const GLTextureCube& other) {
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
  wrapY = other.wrapY;
  wrapZ = other.wrapZ;

  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
  
  if (other.height > 0 && other.width > 0) {
    for (size_t face = 0;face<6;++face) {
      switch (other.dataType) {
        case GLDataType::BYTE  :
          setData(other.data, other.width, other.height, Face(face), other.componentCount);
          break;
#ifndef __EMSCRIPTEN__
        case GLDataType::SHORT  :
          setData(other.sdata, other.width, other.height, Face(face), other.componentCount);
          break;
#endif
        case GLDataType::HALF  :
          setData(other.hdata, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::FLOAT :
          setData(other.fdata, other.width, other.height, Face(face), other.componentCount);
          break;
      }
    }
  }
  return *this;
}

const GLuint GLTextureCube::getId() const {
  return id;
}

void GLTextureCube::clear() {
  setEmpty(width,height,componentCount,dataType);
}

void GLTextureCube::setData(const Image& image, Face face) {
  this->data = image.data;
  setData((GLvoid*)(image.data.data()), image.width, image.height, face, image.componentCount, GLDataType::BYTE);
}

void GLTextureCube::setData(const std::vector<GLubyte>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setData(const std::vector<GLfloat>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setData(const std::vector<half::Half>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setEmpty(uint32_t width, uint32_t height, uint8_t componentCount, GLDataType dataType) {
  for (size_t face = 0;face<6;++face) {
    switch (dataType) {
      case GLDataType::BYTE  :
        setData(std::vector<GLubyte>(width*height*componentCount),
                width, height, Face(face), componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(std::vector<GLushort>(width*height*componentCount),
                width, height, Face(face), componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(std::vector<half::Half>(width*height*componentCount),
                width, height, Face(face), componentCount);
        break;
      case GLDataType::FLOAT :
        setData(std::vector<GLfloat>(width*height*componentCount),
                width, height, Face(face), componentCount);
        break;
    }
  }
}

void GLTextureCube::setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, Face face, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->data = data;
  setData((GLvoid*)data.data(), width, height, face, componentCount, GLDataType::BYTE);
}

#ifndef __EMSCRIPTEN__
void GLTextureCube::setData(const std::vector<GLushort>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setData(const std::vector<GLushort>& data, uint32_t width, uint32_t height, Face face, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }

  this->sdata = data;
  setData((GLvoid*)data.data(), width, height, face, componentCount, GLDataType::SHORT);
}
#endif

void GLTextureCube::setData(const std::vector<half::Half>& data, uint32_t width, uint32_t height, Face face, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->hdata = data;
  setData((GLvoid*)data.data(), width, height, face, componentCount, GLDataType::HALF);
}

void GLTextureCube::setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, Face face, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    std::stringstream ss;
    ss << "Data size " << data.size() << " and texure dimensions " << componentCount << "*" << width
       << "*" << height << "=" << componentCount*width*height << " do not match.";
    throw GLException{ss.str()};
  }
  
  this->fdata = data;
  setData((GLvoid*)this->fdata.data(), width, height, face, componentCount, GLDataType::FLOAT);
}

void GLTextureCube::setData(GLvoid* data, uint32_t width, uint32_t height,
                            Face face, uint8_t componentCount,
                            GLDataType dataType) {
  if (this->width != 0 || this->height != 0) {
    if (this->dataType != dataType ||
        this->width != width ||
        this->height != height ||
        this->componentCount != componentCount) {
      throw GLException{"Texture dimensions do not match."};
    }
  }

  this->dataType = dataType;
  this->width = width;
  this->height = height;
  this->componentCount = componentCount;

  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  const GLTexInfo texInfo = dataTypeToGL(dataType, componentCount);

  GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(face), 0,
                  texInfo.internalformat, GLsizei(width), GLsizei(height), 0,
                  texInfo.format, texInfo.type, data));
}

void GLTextureCube::generateMipmap() {
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}

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
