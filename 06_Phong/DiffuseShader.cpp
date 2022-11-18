#include "DiffuseShader.h"

DiffuseShader::DiffuseShader(const Vec3& light, const Vec3& light_diffuse_color)
: light(light), ld(light_diffuse_color)
{
}

Vec3 DiffuseShader::shade(Vertex surface) const {
  const Material& m = surface.material;
  const Vec3 N = Vec3::normalize(surface.normal);
  const Vec3 L = Vec3::normalize(light - surface.position);
  const float d = std::fmaxf(0.0f, Vec3::dot(N, L));
  
  return m.color_diffuse * ld * d;
}


