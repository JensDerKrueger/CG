#pragma once

#include <vector>

#include <GL/glew.h>  
#include <GLFW/glfw3.h>

class GLBuffer {
public:
	GLBuffer(GLenum target);
	~GLBuffer();
	void setData(const std::vector<GLfloat>& data, size_t valuesPerElement,GLenum usage=GL_STATIC_DRAW);
	void setData(const std::vector<GLuint>& data);

  void setData(const float data[], size_t elemCount,
               size_t valuesPerElement,GLenum usage=GL_STATIC_DRAW);
  void setData(const GLuint data[], size_t elemCount);

	void connectVertexAttrib(GLuint location, size_t elemCount,
                           size_t offset=0, GLuint divisor = 0) const;
	void bind() const;
  
private:
	GLenum target;
	GLuint bufferID;
	size_t elemSize;
	size_t stride;
	GLenum type;
};
