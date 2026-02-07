#include "GLDepthBuffer.h"


GLDepthBuffer::GLDepthBuffer(uint32_t width, uint32_t height) :
	id(0),
  width(width),
  height(height)
{
  GL(glGenRenderbuffers(1, &id));
  GL(glBindRenderbuffer(GL_RENDERBUFFER, id));
  GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                           GLsizei(width), GLsizei(height)));
}

GLDepthBuffer::~GLDepthBuffer() {
	GL(glDeleteRenderbuffers(1, &id));
}

const GLuint GLDepthBuffer::getId() const {
	return id;
}

void GLDepthBuffer::setSize(uint32_t width, uint32_t height) {
  this->width = width;
  this->height =height;
  GL(glBindRenderbuffer(GL_RENDERBUFFER, id));
  GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                           GLsizei(width), GLsizei(height)));
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
