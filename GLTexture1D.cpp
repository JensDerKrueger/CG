#include <iostream>
#include "GLTexture1D.h"


GLTexture1D::GLTexture1D(GLint magFilter, GLint minFilter, GLint wrapX,
                         GLDataType dataType) :
id(0),
dataType(dataType),
magFilter(magFilter),
minFilter(minFilter),
wrapX(wrapX),
size(0),
componentCount(0)
{
  GL(glGenTextures(1, &id));

#ifndef __EMSCRIPTEN__
  GL(glBindTexture(GL_TEXTURE_1D, id));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter));
#else
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
#endif
}

GLTexture1D::GLTexture1D(const GLTexture1D& other) :
GLTexture1D(other.magFilter, other.minFilter, other.wrapX, other.dataType)
{
  if (other.size > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.size, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.size, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.size, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.size, other.componentCount);
        break;
    }
  }
}

GLTexture1D& GLTexture1D::operator=(GLTexture1D other) {
  dataType = other.dataType;
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
#ifndef __EMSCRIPTEN__
  GL(glBindTexture(GL_TEXTURE_1D, id));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter));
#else
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
#endif

  if (other.size > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.size, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.size, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.size, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.size, other.componentCount);
        break;
    }
  }
  return *this;
}

GLTexture1D::~GLTexture1D() {
  GL(glDeleteTextures(1, &id));
}

void GLTexture1D::setEmpty(uint32_t size, uint8_t componentCount,
                           GLDataType dataType) {
  switch (dataType) {
    case GLDataType::BYTE  :
      setData(std::vector<GLubyte>(size*componentCount), size, componentCount);
      break;
#ifndef __EMSCRIPTEN__
    case GLDataType::SHORT  :
      setData(std::vector<GLushort>(size*componentCount), size, componentCount);
      break;
#endif
    case GLDataType::HALF  :
      setData(std::vector<half::Half>(size*componentCount), size, componentCount);
      break;
    case GLDataType::FLOAT :
      setData(std::vector<GLfloat>(size*componentCount), size, componentCount);
      break;
  }
}

void GLTexture1D::clear() {
  setEmpty(size,componentCount,dataType);
}

const GLuint GLTexture1D::getId() const {
  return id;
}

void GLTexture1D::setData(const std::vector<GLubyte>& data) {
  setData(data,size,componentCount);
}

void GLTexture1D::setData(const std::vector<GLubyte>& data, uint32_t size,
                          uint8_t componentCount) {

  if (data.size() != componentCount*size) {
    throw GLException{"Data size and texture dimension do not match."};
  }
  this->data = data;
  setData((GLvoid*)data.data(), size, componentCount, GLDataType::BYTE);
}

#ifndef __EMSCRIPTEN__
void GLTexture1D::setData(const std::vector<GLushort>& data) {
  setData(data,size,componentCount);
}

void GLTexture1D::setData(const std::vector<GLushort>& data, uint32_t size,
                          uint8_t componentCount) {

  if (data.size() != componentCount*size) {
    throw GLException{"Data size and texture dimension do not match."};
  }
  this->sdata = data;
  setData((GLvoid*)data.data(), size, componentCount, GLDataType::SHORT);
}

void GLTexture1D::setData(const std::vector<half::Half>& data) {
  setData(data,size,componentCount);
}
#endif

void GLTexture1D::setData(const std::vector<half::Half>& data, uint32_t size,
                          uint8_t componentCount) {

  if (data.size() != componentCount*size) {
    throw GLException{"Data size and texture dimension do not match."};
  }
  this->hdata = data;
  setData((GLvoid*)data.data(), size, componentCount, GLDataType::HALF);
}

void GLTexture1D::setData(const std::vector<GLfloat>& data) {
  setData(data,size,componentCount);
}

void GLTexture1D::setData(const std::vector<GLfloat>& data, uint32_t size,
                          uint8_t componentCount) {

  if (data.size() != componentCount*size) {
    throw GLException{"Data size and texture dimension do not match."};
  }
  this->fdata = data;
  setData((GLvoid*)data.data(), size, componentCount, GLDataType::FLOAT);
}

void GLTexture1D::setData(GLvoid* data, uint32_t size,
                          uint8_t componentCount, GLDataType dataType) {
  this->size = size;
  this->componentCount = componentCount;
#ifndef __EMSCRIPTEN__
  GL(glBindTexture(GL_TEXTURE_1D, id));
#else
  GL(glBindTexture(GL_TEXTURE_2D, id));
#endif

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  const GLTexInfo texInfo = dataTypeToGL(dataType, componentCount);

#ifndef __EMSCRIPTEN__
  GL(glTexImage1D(GL_TEXTURE_1D, 0, texInfo.internalformat,
                  GLsizei(size), 0, texInfo.format, texInfo.type,
                  data));
#else
  GL(glTexImage2D(GL_TEXTURE_2D, 0, texInfo.internalformat,
                  GLsizei(size), 1, 0, texInfo.format, texInfo.type,
                  data));
#endif
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

