#include <sstream>

#include "GLBuffer.h"
#include "GLEnv.h"


GLBuffer::GLBuffer(GLenum target) :
	target(target),
	bufferID(0),
	elemSize(0),
	stride(0),
	type(0)
{
	GL(glGenBuffers(1, &bufferID));
}

GLBuffer::~GLBuffer()  {
	GL(glBindBuffer(target, 0));
	GL(glDeleteBuffers(1, &bufferID));
}

void GLBuffer::setData(const std::vector<float>& data, size_t valuesPerElement, GLenum usage) {
	elemSize = sizeof(data[0]);
	stride = valuesPerElement*elemSize;
	type = GL_FLOAT;
	GL(glBindBuffer(target, bufferID));
	GL(glBufferData(target, GLsizeiptr(elemSize*data.size()), data.data(), usage));
}

void GLBuffer::setData(const std::vector<GLuint>& data) {
	elemSize = sizeof(data[0]);
	stride = 1*elemSize;
	type = GL_UNSIGNED_INT;
	GL(glBindBuffer(target, bufferID));
	GL(glBufferData(target, GLsizeiptr(elemSize*data.size()), data.data(),
                  GL_STATIC_DRAW));
}

void GLBuffer::setData(const float data[], size_t elemCount,
                       size_t valuesPerElement,GLenum usage) {
  elemSize = sizeof(data[0]);
  stride = valuesPerElement*elemSize;
  type = GL_FLOAT;
  GL(glBindBuffer(target, bufferID));
  GL(glBufferData(target, GLsizeiptr(elemSize*elemCount), data, usage));
}

void GLBuffer::setData(const GLuint data[], size_t elemCount) {
  elemSize = sizeof(data[0]);
  stride = 1*elemSize;
  type = GL_UNSIGNED_INT;
  GL(glBindBuffer(target, bufferID));
  GL(glBufferData(target, GLsizeiptr(elemSize*elemCount), data, GL_STATIC_DRAW));
}


void GLBuffer::connectVertexAttrib(GLuint location, size_t elemCount,
                                   size_t offset, GLuint divisor) const {
    if (type == 0) {
        throw GLException{"Need to call setData before connectVertexAttrib"};
    }
    
	GL(glBindBuffer(target, bufferID));
	GL(glEnableVertexAttribArray(location));
	GL(glVertexAttribPointer(location, GLsizei(elemCount), type, GL_FALSE, GLsizei(stride), (void*)(offset*elemSize)));
  if (divisor != 0) GL(glVertexAttribDivisor(location, divisor));
}

void GLBuffer::bind() const {
	GL(glBindBuffer(target, bufferID));
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
