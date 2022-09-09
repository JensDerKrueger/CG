#include "PointLight.h"


Vec3 PointLight::getDirection(const Vec3& position) const
{
    return Vec3::normalize(this->position - position);
}


double PointLight::getDistance(const Vec3& position) const
{
    return (this->position - position).length();
}
