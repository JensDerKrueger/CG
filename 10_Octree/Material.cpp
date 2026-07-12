#include "Material.h"

Vec3 Material::getAmbient() const
{
    return ambient;
}

Vec3 Material::getDiffuse() const
{
    return diffuse;
}

Vec3 Material::getSpecular() const
{
    return specular;
}

float Material::getExp() const
{
    return exponent;
}

bool Material::isShadowCaster() const
{
    return m_isShadowCaster;
}

bool Material::reflects() const
{
    return local < 1.0;
}

bool Material::refracts() const
{
    return reflects() && IOR;
}

std::optional<float> Material::getIndexOfRefraction() const
{
    return IOR;
}

float Material::getLocalRefectivity() const
{
    return local;
}

float Material::getReflectivity(float cosI) const
{
  const float sign = (cosI < 0) ? -1.0f : 1.0f;

  float R0;
  if (IOR) {
    float n = (sign == 1.0f) ? IOR.value() : 1.0f / IOR.value();
    float R0sqrt = (n - 1.0f) / (n + 1.0f);
    R0 = R0sqrt * R0sqrt;
  } else {
    R0 = 1 - local;
  }

  return R0 + (1.0f - R0) * pow(1.0f - sign * cosI, 5.0f);
}
