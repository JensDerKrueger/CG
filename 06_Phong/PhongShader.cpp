#include "PhongShader.h"
#include <cmath>
#include <algorithm>

// TODO: Implement the missing methods and the rest of this class as necessary for the assignment.

PhongShader::PhongShader(const Vec3& viewer, const Vec3& light, const Vec3& light_ambient_color, const Vec3& light_diffuse_color, const Vec3& light_specular_color, float exponent)
{
  // TODO
}

PhongShader::PhongShader(const PhongShader& other)
{
  // TODO
}

Vec3 PhongShader::shade(Vertex surface) const
{
  // TODO
  return Vec3(0, 0, 0); // just a dummy return value, needs to be replaced with the right one
}

