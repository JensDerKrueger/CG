#include "GLArray.h"

GLArray::GLArray() {
	GL(glGenVertexArrays(1, &glId));
	GL(glBindVertexArray(glId));
}

GLArray::~GLArray() {
	GL(glDeleteVertexArrays(1, &glId));
}

void GLArray::bind() const {
	GL(glBindVertexArray(glId));
}

void GLArray::connectVertexAttrib(const GLBuffer& buffer,
                                  const GLProgram& program,
                                  const std::string& variable,
                                  size_t elemCount, size_t offset,
                                  GLuint divisor) const {
	bind();
	const GLint location = program.getAttributeLocation(variable.c_str());
  // TODO: consider handling -1 location, maybe throw an exception?
	buffer.connectVertexAttrib(GLuint(location), elemCount, offset, divisor);
}

void GLArray::connectIndexBuffer(const GLBuffer& buffer) const {
	bind();
	buffer.bind();	
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
