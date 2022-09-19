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
    return reflects() && (IOR.has_value());
}

bool Material::hasTexture() const
{
    return texture.has_value();
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
    float R0 = 1 - local;
    int sign = (cosI < 0) ? -1 : 1;

    if (IOR.has_value())
    {
        float n = (sign == 1) ? IOR.value() : 1.0 / IOR.value();
        float R0sqrt = (n - 1) / (n + 1);
        R0 = R0sqrt * R0sqrt;
    }

    return R0 + (1.0 - R0) * pow(1.0f - sign * cosI, 5);
}

std::optional<Texture> Material::getTexture() const
{
    return texture;
}
