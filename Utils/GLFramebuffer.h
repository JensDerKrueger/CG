#pragma once

#include <vector>

#include "GLEnv.h"

#include "GLTexture2D.h"
#include "GLTexture3D.h"
#include "GLDepthBuffer.h"
#include "GLDepthTexture.h"

class GLFramebuffer {
public:
  GLFramebuffer();
  ~GLFramebuffer();

  const GLuint getId() const;

  void bind(const GLTexture2D& t, const GLDepthBuffer& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthBuffer& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthBuffer& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthBuffer& d);

  void bind(const GLTexture2D& t);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3);


  void bind(const GLDepthTexture& d);
  void bind(const GLTexture2D& t, const GLDepthTexture& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthTexture& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthTexture& d);
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthTexture& d);

  void bind(const GLTexture3D& t, size_t slice, const GLDepthBuffer& d);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLDepthBuffer& d);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLDepthBuffer& d);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLTexture3D& t3, size_t slice3, const GLDepthBuffer& d);

  void bind(const GLTexture3D& t, size_t slice);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2);
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLTexture3D& t3, size_t slice3);

  void unbind2D();
  void unbind3D();
  bool checkBinding() const;
    
private:
  GLuint id;
  
  void setBuffers(size_t count, size_t width, size_t height);
};

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
