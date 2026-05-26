#include <array>
#include <sstream>

#include "GLTexture2D.h"

GLTexture2D::GLTexture2D(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY) :
  id(0),
  magFilter(magFilter),
  minFilter(minFilter),
  wrapX(wrapX),
  wrapY(wrapY),
  width(0),
  height(0),
  componentCount(0),
  dataType(GLDataType::BYTE)
{
  GL(glGenTextures(1, &id));
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTexture2D::GLTexture2D(const Image& image,
                         GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY) :
GLTexture2D(magFilter, minFilter, wrapX, wrapY)
{
  setData(image);
}

void GLTexture2D::setFilter(GLint magFilter, GLint minFilter) {
  this->magFilter = magFilter;
  this->minFilter = minFilter;
  
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTexture2D::~GLTexture2D() {
  GL(glDeleteTextures(1, &id));
}

GLTexture2D::GLTexture2D(const GLTexture2D& other) :
  GLTexture2D(other.magFilter, other.minFilter, other.wrapX, other.wrapY)
{
  if (other.height > 0 && other.width > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.width, other.height, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.componentCount);
        break;
    }
  }
}

GLTexture2D& GLTexture2D::operator=(const GLTexture2D& other) {
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
  wrapY = other.wrapY;

  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
  
  if (other.height > 0 && other.width > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.componentCount);
        break;
#ifndef __EMSCRIPTEN__
      case GLDataType::SHORT  :
        setData(other.sdata, other.width, other.height, other.componentCount);
        break;
#endif
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.componentCount);
        break;
    }
  }
  return *this;
}

const GLuint GLTexture2D::getId() const {
  return id;
}

void GLTexture2D::clear() {
  setEmpty(width,height,componentCount,dataType);
}

void GLTexture2D::setData(const Image& image) {
  this->data = image.data;
  setData((GLvoid*)(image.data.data()), image.width,
          image.height, image.componentCount, GLDataType::BYTE);
}

void GLTexture2D::setData(const std::vector<GLubyte>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setData(const std::vector<half::Half>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setData(const std::vector<GLfloat>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setEmpty(uint32_t width, uint32_t height, uint8_t componentCount, GLDataType dataType) {
  switch (dataType) {
    case GLDataType::BYTE  :
      setData(std::vector<GLubyte>(width*height*componentCount), width, height, componentCount);
      break;
#ifndef __EMSCRIPTEN__
    case GLDataType::SHORT  :
      setData(std::vector<GLushort>(width*height*componentCount), width, height, componentCount);
      break;
#endif
    case GLDataType::HALF  :
      setData(std::vector<half::Half>(width*height*componentCount), width, height, componentCount);
      break;
    case GLDataType::FLOAT :
      setData(std::vector<GLfloat>(width*height*componentCount), width, height, componentCount);
      break;
  }
}

void GLTexture2D::setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texture dimensions do not match."};
  }
  
  this->data = data;
  setData((GLvoid*)data.data(), width, height, componentCount, GLDataType::BYTE);
}

#ifndef __EMSCRIPTEN__
void GLTexture2D::setData(const std::vector<GLushort>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setData(const std::vector<GLushort>& data, uint32_t width, uint32_t height, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texture dimensions do not match."};
  }

  this->sdata = data;
  setData((GLvoid*)data.data(), width, height, componentCount, GLDataType::BYTE);
}
#endif

void GLTexture2D::setData(const std::vector<half::Half>& data, uint32_t width, uint32_t height, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texture dimensions do not match."};
  }
  
  this->hdata = data;
  setData((GLvoid*)data.data(), width, height, componentCount, GLDataType::HALF);
}

void GLTexture2D::setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint8_t componentCount) {
  if (data.size() != componentCount*width*height) {
    std::stringstream ss;
    ss << "Data size " << data.size() << " and texture dimensions " << componentCount << "*" << width
       << "*" << height << "=" << componentCount*width*height << " do not match.";
    throw GLException{ss.str()};
  }
  
  this->fdata = data;
  setData((GLvoid*)this->fdata.data(), width, height, componentCount, GLDataType::FLOAT);
}

void GLTexture2D::setData(GLvoid* data, uint32_t width, uint32_t height,
                          uint8_t componentCount, GLDataType dataType) {
  this->dataType = dataType;
  this->width = width;
  this->height = height;
  this->componentCount = componentCount;

  GL(glBindTexture(GL_TEXTURE_2D, id));

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  const GLTexInfo texInfo = dataTypeToGL(dataType, componentCount);

  GL(glTexImage2D(GL_TEXTURE_2D, 0, texInfo.internalformat, GLsizei(width), GLsizei(height), 0, texInfo.format, texInfo.type, data));
}


void GLTexture2D::setPixel(const std::vector<GLubyte>& data, uint32_t x, uint32_t y) {  
  const GLTexInfo texInfo = dataTypeToGL(dataType, componentCount);
  GL(glBindTexture(GL_TEXTURE_2D, id));
  glTexSubImage2D(GL_TEXTURE_2D,0,GLint(x),GLint(y),1,1, texInfo.format,
                  texInfo.type, data.data());
}

void GLTexture2D::generateMipmap() {
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glGenerateMipmap(GL_TEXTURE_2D));
}

#ifndef __EMSCRIPTEN__

Image GLTexture2D::getImage() {
  return {width, height, componentCount, getDataByte()};
}

const std::vector<GLubyte>& GLTexture2D::getDataByte() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));
  
  const GLTexInfo texInfo = dataTypeToGL(GLDataType::BYTE, componentCount);
  data.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, texInfo.format, texInfo.type, data.data()));
  
  return data;
}

const std::vector<GLushort>& GLTexture2D::getDataShort() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));

  const GLTexInfo texInfo = dataTypeToGL(GLDataType::BYTE, componentCount);
  sdata.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, texInfo.format, texInfo.type, data.data()));

  return sdata;
}

const std::vector<half::Half>& GLTexture2D::getDataHalf() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));

  const GLTexInfo texInfo = dataTypeToGL(GLDataType::HALF, componentCount);
  hdata.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, texInfo.format, texInfo.type,
                   hdata.data()));
  return hdata;
}

const std::vector<GLfloat>& GLTexture2D::getDataFloat() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));

  const GLTexInfo texInfo = dataTypeToGL(GLDataType::FLOAT, componentCount);
  fdata.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, texInfo.format, texInfo.type,
                   fdata.data()));
  return fdata;
}

#endif

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
