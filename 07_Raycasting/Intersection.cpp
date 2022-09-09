#include "Intersection.h"

Material Intersection::getMaterial() const
{
    return material;
}

Vec3 Intersection::getNormal() const
{
    return normal;
}

double Intersection::getT() const
{
    return t;
}
