#include "Plane.h"

Material Plane::getMaterial() const
{
    return material;
}

std::optional<Intersection> Plane::intersect(const Ray& ray) const
{
    double denom = Vec3::dot(ray.getDirection(), normal);
    if (denom == 0)
        return {};

    double t = -(Vec3::dot(ray.getOrigin(), normal) + d) / denom;
    if (t < 0)
        return {};

    return Intersection{ material, normal, t };
}
