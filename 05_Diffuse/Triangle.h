#pragma once

#include "Vertex.h"
#include "Image.h"
#include "Shader.h"

class Triangle {
public:
  Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Shader& s);
  void draw(Image& image);

private:
  // TODO: add the appropriate member variables here

};
