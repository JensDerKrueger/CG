#include "GLTexture3D.h"

GLTexture3D::GLTexture3D(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY, GLint wrapZ, GLDataType dataType) :
id(0),
magFilter(magFilter),
minFilter(minFilter),
wrapX(wrapX),
wrapY(wrapY),
wrapZ(wrapZ),
width(0),
height(0),
depth(0),
componentCount(0),
dataType(dataType)
{
  GL(glGenTextures(1, &id));
  GL(glBindTexture(GL_TEXTURE_3D, id));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTexture3D::~GLTexture3D() {
  GL(glDeleteTextures(1, &id));
}

GLTexture3D::GLTexture3D(const GLTexture3D& other) :
GLTexture3D(other.magFilter,
            other.minFilter,
            other.wrapX,
            other.wrapY,
            other.wrapZ,
            other.dataType)
{
  if (other.height > 0 && other.width > 0 && other.depth > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.depth, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.width, other.height, other.depth, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.depth, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.depth, other.componentCount);
        break;
    }
  }
}

GLTexture3D& GLTexture3D::operator=(GLTexture3D other) {
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
  wrapY = other.wrapY;
  wrapZ = other.wrapZ;
  dataType = other.dataType;

  GL(glBindTexture(GL_TEXTURE_3D, id));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));

  if (other.height > 0 && other.width > 0 && other.depth > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.depth, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.width, other.height, other.depth, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.depth, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.depth, other.componentCount);
        break;
    }
  }
  return *this;
}

void GLTexture3D::setFilter(GLint magFilter, GLint minFilter) {
  this->magFilter = magFilter;
  this->minFilter = minFilter;

  GL(glBindTexture(GL_TEXTURE_3D, id));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));
}

const GLuint GLTexture3D::getId() const {
  return id;
}

void GLTexture3D::clear() {
  setEmpty(width,height,depth,componentCount,dataType);
}

void GLTexture3D::setData(const std::vector<GLubyte>& data) {
  setData(data,width,height,depth,componentCount);
}

void GLTexture3D::setEmpty(uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount, GLDataType dataType) {

  switch (dataType) {
    case GLDataType::BYTE  :
      setData(std::vector<GLubyte>(width*height*depth*componentCount), width, height, depth, componentCount);
      break;
#ifndef __EMSCRIPTEN__
    case GLDataType::SHORT  :
      setData(std::vector<GLushort>(width*height*depth*componentCount), width, height, depth, componentCount);
      break;
#endif
    case GLDataType::HALF  :
      setData(std::vector<half::Half>(width*height*depth*componentCount), width, height, depth, componentCount);
      break;
    case GLDataType::FLOAT :
      setData(std::vector<GLfloat>(width*height*depth*componentCount), width, height, depth, componentCount);
      break;
  }
}

void GLTexture3D::setData(const std::vector<GLubyte>& data,
                          uint32_t width, uint32_t height,
                          uint32_t depth, uint8_t componentCount) {
  if (data.size() != componentCount*width*height*depth) {
    throw GLException{"Data size and texture dimensions do not match."};
  }

  this->data = data;
  setData((GLvoid*)data.data(),
          width,
          height,
          depth,
          componentCount,
          GLDataType::BYTE);
}

void GLTexture3D::setData(const std::vector<GLfloat>& data) {
  setData(data,width,height,depth,componentCount);
}

#ifndef __EMSCRIPTEN__
void GLTexture3D::setData(const std::vector<GLushort>& data,
                          uint32_t width, uint32_t height,
                          uint32_t depth, uint8_t componentCount) {
  if (data.size() != componentCount*width*height*depth) {
    throw GLException{"Data size and texture dimensions do not match."};
  }

  this->sdata = data;
  setData((GLvoid*)data.data(),
          width,
          height,
          depth,
          componentCount,
          GLDataType::SHORT);
}

void GLTexture3D::setData(const std::vector<GLushort>& data) {
  setData(data,width,height,depth,componentCount);
}
#endif

void GLTexture3D::setData(const std::vector<half::Half>& data, uint32_t width,
                          uint32_t height, uint32_t depth, uint8_t componentCount) {
  if (data.size() != componentCount*width*height*depth) {
    throw GLException{"Data size and texture dimensions do not match."};
  }
  this->hdata = data;
  setData((GLvoid*)data.data(),
          width,
          height,
          depth,
          componentCount,
          GLDataType::HALF);
}

void GLTexture3D::setData(const std::vector<half::Half>& data) {
  setData(data,width,height,depth,componentCount);
}


void GLTexture3D::setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount) {
  if (data.size() != componentCount*width*height*depth) {
    throw GLException{"Data size and texture dimensions do not match."};
  }
  this->fdata = data;
  setData((GLvoid*)data.data(),
          width,
          height,
          depth,
          componentCount,
          GLDataType::FLOAT);
}

void GLTexture3D::setData(const GLvoid* data, uint32_t width, uint32_t height,
                          uint32_t depth, uint8_t componentCount,
                          GLDataType dataType) {
  this->dataType = dataType;
  this->width = width;
  this->height = height;
  this->depth = depth;
  this->componentCount = componentCount;

  GL(glBindTexture(GL_TEXTURE_3D, id));

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  const GLTexInfo texInfo = dataTypeToGL(dataType, componentCount);

  GL(glTexImage3D(GL_TEXTURE_3D, 0, texInfo.internalformat,
                  GLsizei(width), GLsizei(height), GLsizei(depth),
                  0, texInfo.format, texInfo.type, data));
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
