#include "Intersection.h"

Material Intersection::getMaterial() const
{
    return material;
}

Vec3 Intersection::getNormal() const
{
    return normal;
}

float Intersection::getT() const
{
    return t;
}

std::optional<TextureCoordinates> Intersection::getTexCoords() const
{
    return texCoords;
}
