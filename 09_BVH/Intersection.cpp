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
