#include "Plane.h"

Material Plane::getMaterial() const
{
    return material;
}

std::optional<Intersection> Plane::intersect(const Ray& ray) const
{
    double denom = Vec3::dot(ray.getDirection(), normal);
    if (denom == 0.0)
        return {};

    double t = -(Vec3::dot(ray.getOrigin(), normal) + d) / denom;
    if (t < 0)
        return {};

    TextureCoordinates tc(0.0f, 0.0f);
    if (material.hasTexture())
    {
        Vec3 p = ray.getPosOnRay(t);
        p - center;
        tc = TextureCoordinates(Vec3::dot(p, frame1), Vec3::dot(p, frame2));
    }
    return Intersection{ material, normal, tc, t };
}

void Plane::buildLocalFrame()
{
    Vec3 v(normal);

    // construct vector not colinear to normal
    if (v[0] != v[1])
    {
        float temp = v[0];
        v[0] = v[1];
        v[1] = temp;
    }
    else if (v[0] != v[2])
    {
        float temp = v[0];
        v[0] = v[2];
        v[2] = temp;
    }
    else if (v[1] != v[2])
    {
        float temp = v[1];
        v[1] = v[2];
        v[2] = temp;
    }
    else
    {
        v = Vec3(0, 1, 0);
    }

    //build frame
    frame1 = Vec3::normalize(Vec3::cross(normal, v));
    frame2 = Vec3::normalize(Vec3::cross(normal, frame1));

    center = Vec3(0.0f, 0.0f, 0.0f) + (normal * -d);
}
