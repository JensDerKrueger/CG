#include <iostream>
#include "GLTexture1D.h"


GLTexture1D::GLTexture1D(GLint magFilter, GLint minFilter, GLint wrapX) :
	id(0),
	internalformat(0),
	format(0),
	type(0),
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
    GLTexture1D(other.magFilter, other.minFilter, other.wrapX)
{
    setData(other.data, other.size, other.componentCount);
}

GLTexture1D& GLTexture1D::operator=(GLTexture1D other) {
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
    setData(other.data, other.size, other.componentCount);
    return *this;
}

GLTexture1D::~GLTexture1D() {
	GL(glDeleteTextures(1, &id));
}


const GLuint GLTexture1D::getId() const {
	return id;
}

void GLTexture1D::setData(const std::vector<GLubyte>& data, uint32_t size, 
                          uint8_t componentCount) {
	if (data.size() != componentCount*size) {
		throw GLException{"Data size and texture dimensions do not match."};
	}
	
    this->data = data;
    this->size = size;
    this->componentCount = componentCount;
#ifndef __EMSCRIPTEN__
	GL(glBindTexture(GL_TEXTURE_1D, id));
#else
  GL(glBindTexture(GL_TEXTURE_2D, id));
#endif

	GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
	GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));
	
	type = GL_UNSIGNED_BYTE;	
	switch (componentCount) {
		case 1 : 
			internalformat = GL_R8;
			format = GL_RED;
			break;
		case 2 : 
			internalformat = GL_RG8;
			format = GL_RG;
			break;
		case 3 : 
			internalformat = GL_RGB8;
			format = GL_RGB;
			break;
		case 4 : 
			internalformat = GL_RGBA8;
			format = GL_RGBA;
			break;
	} 

#ifndef __EMSCRIPTEN__
	GL(glTexImage1D(GL_TEXTURE_1D, 0, internalformat, GLsizei(size), 0, format, type, (GLvoid*)data.data()));
#else
  GL(glTexImage2D(GL_TEXTURE_2D, 0, internalformat, GLsizei(size), 1, 0, format, type, (GLvoid*)data.data()));
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
