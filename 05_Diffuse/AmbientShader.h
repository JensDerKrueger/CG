#pragma once
#include "Shader.h"

class AmbientShader : public Shader {
public:
  Vec3 shade(Vertex surface) const override;
  virtual ~AmbientShader() {}
};
