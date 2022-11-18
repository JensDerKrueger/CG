#include "Triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                   const Shader& s)
// TODO: initialize member variables here
{
}

void Triangle::draw(Image& image) {
  // TODO:
  // Rasterize the triangle into the image. A possible, yet not very efficient,
  // solution would be to compute for every pixel in the image, whether it is
  // inside this trianhle or not. If the pixel is inside the triangle you
  // should assign a color to the piel using the shader of the triangle
}
