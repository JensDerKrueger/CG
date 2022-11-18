#pragma once

#include "Vec3.h"
#include "Vertex.h"

class Shader {
public:
  virtual Vec3 shade(Vertex surface) const = 0;
  virtual ~Shader() {}
};
