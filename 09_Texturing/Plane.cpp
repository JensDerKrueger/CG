#include "Plane.h"
#include <cmath>

Material Plane::getMaterial() const
{
    return material;
}

std::optional<Intersection> Plane::intersect(const Ray& ray) const
{
    float denom = Vec3::dot(ray.getDirection(), normal);
    if (denom == 0.0)
        return {};

    float t = -(Vec3::dot(ray.getOrigin(), normal) + d) / denom;
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

    // construct vector not collinear to normal
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

Tessellation Plane::getMesh() const
{
    constexpr float planeSize = 1000.0f;

    const Vec3 n = Vec3::normalize(normal);
    const Vec3 center = normal * (-d / Vec3::dot(normal, normal));
    const Vec3 helper = (std::fabs(n.y) < 0.9f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
    const Vec3 tangent = Vec3::normalize(Vec3::cross(n, helper));
    const Vec3 bitangent = Vec3::cross(n, tangent);

    return Tessellation::genRectangle(center - tangent * planeSize - bitangent * planeSize,
                                      center + tangent * planeSize - bitangent * planeSize,
                                      center + tangent * planeSize + bitangent * planeSize,
                                      center - tangent * planeSize + bitangent * planeSize);
}
