#include "GLFramebuffer.h"


GLFramebuffer::GLFramebuffer() :
  id(0)
{
    GL(glGenFramebuffers(1, &id));
}

GLFramebuffer::~GLFramebuffer() {
    GL(glDeleteFramebuffers(1, &id));
}

void GLFramebuffer::bind(const GLTexture2D& t, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.getId(), 0));
#endif
  setBuffers(1, t.getWidth(), t.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
#endif
  setBuffers(2, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
#endif
  setBuffers(3, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, t3.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, t3.getId(), 0));
#endif
  setBuffers(4, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLDepthTexture& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d.getId(),0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d.getId(), 0));
#endif
  setBuffers(0, d.getWidth(), d.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t, const GLDepthTexture& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));

#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d.getId(),0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.getId(), 0));
#endif
  setBuffers(1, t.getWidth(), t.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthTexture& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d.getId(),0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d.getId(),0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
#endif
  setBuffers(2, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthTexture& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d.getId(),0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d.getId(),0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
#endif
  setBuffers(3, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthTexture& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d.getId(),0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, t3.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d.getId(),0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, t3.getId(), 0));
#endif
  setBuffers(4, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.getId(), 0));
#endif
  setBuffers(1, t.getWidth(), t.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
#endif
  setBuffers(2, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
#endif
  setBuffers(3, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t0.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, t1.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, t2.getId(), 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, t3.getId(), 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t0.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, t1.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, t2.getId(), 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, t3.getId(), 0));
#endif
  setBuffers(4, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t, size_t slice, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t.getId(), 0, GLint(slice)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t.getId(), 0, GLint(slice)));
#endif

  setBuffers(1,t.getWidth(), t.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
#endif
  setBuffers(2, t0.getWidth(), t0.getHeight());
}
void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_3D, t2.getId(), 0, GLint(slice2)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t2.getId(), 0, GLint(slice2)));
#endif

  setBuffers(3, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
                         const GLTexture3D& t3, size_t slice3, const GLDepthBuffer& d) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d.getId()));

#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_3D, t2.getId(), 0, GLint(slice2)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_3D, t3.getId(), 0, GLint(slice3)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t2.getId(), 0, GLint(slice2)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t3.getId(), 0, GLint(slice3)));
#endif

  setBuffers(4, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t, size_t slice) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t.getId(), 0, GLint(slice)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t.getId(), 0, GLint(slice)));
#endif
  setBuffers(1,t.getWidth(), t.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
#endif
  setBuffers(2, t0.getWidth(), t0.getHeight());
}
void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_3D, t2.getId(), 0, GLint(slice2)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t2.getId(), 0, GLint(slice2)));
#endif

  setBuffers(3, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2, const GLTexture3D& t3, size_t slice3) {
  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
  GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));

#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_3D, t2.getId(), 0, GLint(slice2)));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_3D, t3.getId(), 0, GLint(slice3)));
#else
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, t0.getId(), 0, GLint(slice0)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, t1.getId(), 0, GLint(slice1)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t2.getId(), 0, GLint(slice2)));
  GL(glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, t3.getId(), 0, GLint(slice3)));
#endif

  setBuffers(4, t0.getWidth(), t0.getHeight());
}

void GLFramebuffer::unbind2D() {
  GLenum bufs[] = { GL_NONE };
  glDrawBuffers(1, bufs);
  glReadBuffer(GL_NONE);

  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, 0, 0, 0));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_3D, 0, 0, 0));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_3D, 0, 0, 0));
  GL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_3D, 0, 0, 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0));
#endif
  GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void GLFramebuffer::unbind3D() {
  GLenum bufs[] = { GL_NONE };
  glDrawBuffers(1, bufs);
  glReadBuffer(GL_NONE);

  GL(glBindFramebuffer(GL_FRAMEBUFFER, id));
#ifndef __EMSCRIPTEN__
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0));
  GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0));
#else
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0));
  GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0));
#endif
  GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

bool GLFramebuffer::checkBinding() const {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

const GLuint GLFramebuffer::getId() const {
  return id;
}

void GLFramebuffer::setBuffers(size_t count, size_t width, size_t height) {
  switch (count) {
    case 0: {
      GLenum DrawBuffers[] = { GL_NONE };
      glDrawBuffers(1, DrawBuffers);
      break;
    }
    case 1 : {
      GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
      GL(glDrawBuffers(1, DrawBuffers));
      break;
    }
    case 2 : {
      GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
      GL(glDrawBuffers(2, DrawBuffers));
      break;
    }
    case 3 :{
      GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
      GL(glDrawBuffers(3, DrawBuffers));
      break;
    }
    case 4 :{
      GLenum DrawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
      GL(glDrawBuffers(4, DrawBuffers));
      break;
    }
  }
  
  GL(glViewport(0, 0, GLint(width), GLint(height)));
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
