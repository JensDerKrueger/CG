#include "PointLight.h"


Vec3 PointLight::getDirection(const Vec3& position) const
{
    return Vec3::normalize(this->position - position);
}


float PointLight::getDistance(const Vec3& position) const
{
    return (this->position - position).length();
}
