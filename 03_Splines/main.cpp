#include <iostream>
#include <GLApp.h>
#include <Vec2.h>
#include <Vec4.h>
#include <Mat4.h>

class MyGLApp : public GLApp {
public:
  double sa = 0;
  double ca = 0;
  const size_t maxLineSegments = 100;

  MyGLApp() : GLApp{800,800,1,"Spline Demo"} {}
  
  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    setBackground(1, 1, 1, 1);
    setAnimation(true);
  }
  
  virtual void animate(double animationTime) override {
    sa = sin(animationTime);
    ca = cos(animationTime);
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action != GLENV_PRESS) return;

    switch (key) {
      case GLENV_KEY_SPACE:
        setAnimation(!getAnimation());
        break;
      case GLENV_KEY_ESCAPE:
        closeWindow();
        break;
    }
  }
   
  void drawPolySegment(const Vec2& p0, const Vec2& p1,
                       const Vec2& p2, const Vec2& p3,
                       const Mat4& g, const Vec4& color) {
    std::vector<float> curve((maxLineSegments+1)*7);
    
    for (size_t i = 0;i<=maxLineSegments;++i) {
     const float t = float(i)/float(maxLineSegments);

      // TODO:
      // Complete the function drawPolySegment.
      // This function takes as arguments the geometry matrix of the
      // polygon method, i.e. Hermite, Bezier, or B-spline, and draws
      // the polygonal curve as a line strip. The curve is given as
      // five parameters, i.e. the four control points or, in the case
      // of the Hermite curve, two points and two derivative vectors,
      // the geometry matrix, and the color of the curve. The result
      // should be written into the vector curve. The format is
      // x, y, z, r, g, b, a for each point along the line.
      // The result will be three curves: a Hermite curve on the top,
      // a Bezier curve in the middle, and a B-spline at the bottom.

      curve[i*7+0] = 0.0f;  // x
      curve[i*7+1] = 0.0f;  // y
      curve[i*7+2] = 0.0f;  // z  (no need to change)
      
      curve[i*7+3] = 0.0f;  // red
      curve[i*7+4] = 0.0f;  // green
      curve[i*7+5] = 0.0f;  // blue
      curve[i*7+6] = 0.0f;  // alpha
    }
    drawLines(curve, LineDrawType::STRIP, 5);
  }
 
  void drawHermiteSegment(const Vec2& p0, const Vec2& p1, const Vec2& m0,
                          const Vec2& m1, const Vec4& color) {
    Mat4 g{
      1, 0, 0, 0,
      0, 0, 1, 0,
     -3, 3,-2,-1,
      2,-2, 1, 1
    };
    drawPolySegment(p0,p1,m0,m1,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p0.x+m0.x,p0.y+m0.y,0,0,0,1,1,
               p1.x+m1.x,p1.y+m1.y,0,0,0,1,1,
               p1.x,p1.y,0,1,0,0,1}, 20, true);
  }
  
  void drawBezierSegmentDeCasteljau(const Vec2& p0, const Vec2& p1,
                                    const Vec2& p2, const Vec2& p3,
                                    const Vec4& color) {
    // TODO SOLUTION 2
  }

  void drawBezierSegment(const Vec2& p0, const Vec2& p1, const Vec2& p2,
                         const Vec2& p3, const Vec4& color) {
    Mat4 g{
      1, 0, 0, 0,
     -3, 3, 0, 0,
      3,-6, 3, 0,
     -1, 3,-3, 1
    };
    drawPolySegment(p0,p1,p2,p3,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p1.x,p1.y,0,0,0,1,1,
               p2.x,p2.y,0,0,0,1,1,
               p3.x,p3.y,0,1,0,0,1}, 20, true);
  }
  
  void drawBSplineSegment(const Vec2& p0, const Vec2& p1, const Vec2& p2,
                          const Vec2& p3, const Vec4& color) {
    Mat4 g{
      1/6.0f, 4/6.0f, 1/6.0f, 0/6.0f,
     -3/6.0f, 0/6.0f, 3/6.0f, 0/6.0f,
      3/6.0f,-6/6.0f, 3/6.0f, 0/6.0f,
     -1/6.0f, 3/6.0f,-3/6.0f, 1/6.0f
    };
    drawPolySegment(p0,p1,p2,p3,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p1.x,p1.y,0,0,0,1,1,
               p2.x,p2.y,0,0,0,1,1,
               p3.x,p3.y,0,1,0,0,1}, 20, true);
  }
  
  virtual void draw() override {

    {
      setDrawTransform(Mat4::translation(0.0f,0.7f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 m0{float(sa)*0.2f,float(ca)*0.2f};
      const Vec2 m1{0.0f,-0.2f};
      const Vec2 p1{0.5f,0.0f};
      drawHermiteSegment(p0,p1,m0,m1,{0.0f,0.0f,0.0f,1.0f});
    }
    

    {
      setDrawTransform(Mat4::translation(0.0f,0.0f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBezierSegment(p0,p1,p2,p3,{0.0f,0.0f,0.0f,1.0f});
    }
   
    {
      setDrawTransform(Mat4::translation(0.0f,-0.2f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBezierSegmentDeCasteljau(p0,p1,p2,p3,{0.0f,0.0f,0.0f,1.0f});
    }

    {
      setDrawTransform(Mat4::translation(0.0f,-0.7f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBSplineSegment(p0,p0,p0,p1,{1.0f,0.0f,0.0f,1.0f});
      drawBSplineSegment(p0,p0,p1,p2,{0.0f,1.0f,0.0f,1.0f});
      drawBSplineSegment(p0,p1,p2,p3,{0.0f,0.0f,1.0f,1.0f});
      drawBSplineSegment(p1,p2,p3,p3,{0.0f,1.0f,1.0f,1.0f});
      drawBSplineSegment(p2,p3,p3,p3,{1.0f,0.0f,1.0f,1.0f});
    }
  }
} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
    std::vector<std::string> args{ argv + 1, argv + argc };
#endif
    try {
        myApp.run();
    }
    catch (const GLException& e) {
        std::stringstream ss;
        ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
        std::cerr << ss.str().c_str() << std::endl;
#else
        MessageBoxA(
            NULL,
            ss.str().c_str(),
            "OpenGL Error",
            MB_ICONERROR | MB_OK
        );
#endif
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
