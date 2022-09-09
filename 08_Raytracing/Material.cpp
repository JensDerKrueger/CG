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

double Material::getExp() const
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
    return reflects() && (IOR.has_value());
}

std::optional<float> Material::getIndexOfRefraction() const
{
    return IOR;
}

double Material::getLocalRefectivity() const
{
    return local;
}
