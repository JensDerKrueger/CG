#include "Ray.h"

Vec3 Ray::getOrigin() const
{
    return origin;
}

Vec3 Ray::getDirection() const
{
    return direction;
}

Vec3 Ray::getPosOnRay(double t) const
{
    return origin + direction * t;
}
