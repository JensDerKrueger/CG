#pragma once

#include <vector>

#include "GLEnv.h"  

class GLTexture3D {
public:
	GLTexture3D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT, GLint wrapZ=GL_REPEAT);
	~GLTexture3D();
	
  GLTexture3D(const GLTexture3D& other);
  GLTexture3D& operator=(GLTexture3D other);
    
	const GLuint getId() const;
  void clear();
  void setEmpty(uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount, bool isFloat=false);
	void setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<GLubyte>& data);
  void setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount=4);
  void setData(const std::vector<GLfloat>& data);

  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getDepth() const {return depth;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*depth*componentCount;}
  bool getIsFloat() const {return isFloat;}
  
#ifndef __EMSCRIPTEN__
  const std::vector<GLubyte>& getDataByte();
  const std::vector<GLfloat>& getDataFloat();
#endif

  static uint32_t getMaxSize() {
    GLint max3DSize = 0;
    GL(glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DSize));
    return uint32_t(max3DSize);
  }

  void setData(GLvoid* data, uint32_t width, uint32_t height, uint32_t depth,
               uint8_t componentCount, bool isFloat);

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
  std::vector<GLfloat> fdata;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint8_t componentCount;
  bool isFloat;
  
};
