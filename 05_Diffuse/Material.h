#pragma once
#include "Vec3.h"

class Material {
public:
  Vec3 color_ambient;
  Vec3 color_diffuse;
  Vec3 color_specular;

  Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular)
  : color_ambient(ambient), color_diffuse(diffuse), color_specular(specular)
  { }

  Material(const Vec3& diffuse)
  : Material(diffuse, diffuse, { 1,1,1 })
  { }
};
