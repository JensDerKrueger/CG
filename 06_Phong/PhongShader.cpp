#include "PhongShader.h"
#include <cmath>
#include <algorithm>

PhongShader::PhongShader(const Vec3& viewer, const Vec3& light, const Vec3& light_ambient_color, const Vec3& light_diffuse_color, const Vec3& light_specular_color, float exponent)
{
  // TODO: implement this method and the rest of this class necessary for the assignment
}

PhongShader::PhongShader(const PhongShader& other)
{
  // TODO: implement this method and the rest of this class necessary for the assignment
}

Vec3 PhongShader::shade(Vertex surface) const
{
  // TODO: implement this method and the rest of this class necessary for the assignment
  return Vec3(0, 0, 0); // just a dummy return value, needs to be replaced with the right one
}

