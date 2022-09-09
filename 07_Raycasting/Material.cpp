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
	return local < 1.0f;
}
