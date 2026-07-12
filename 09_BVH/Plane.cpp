#include "Plane.h"
#include <Tessellation.h>
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

    return Intersection{ material, normal, t };
}

void Plane::appendTriangleData(std::vector<float>& data) const
{
    constexpr float planeSize = 1000.0f;

    const Vec3 n = Vec3::normalize(normal);
    const Vec3 center = normal * (-d / Vec3::dot(normal, normal));
    const Vec3 helper = (std::fabs(n.y) < 0.9f) ? Vec3{0.0f, 1.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
    const Vec3 tangent = Vec3::normalize(Vec3::cross(n, helper));
    const Vec3 bitangent = Vec3::cross(n, tangent);

    const Tessellation mesh = Tessellation::genRectangle(center - tangent * planeSize - bitangent * planeSize,
                                                         center + tangent * planeSize - bitangent * planeSize,
                                                         center + tangent * planeSize + bitangent * planeSize,
                                                         center - tangent * planeSize + bitangent * planeSize).unpack();
    const std::vector<float>& vertices = mesh.getVertices();
    const std::vector<float>& normals = mesh.getNormals();
    const Vec3 color = material.getDiffuse();
    const size_t vertexCount = vertices.size() / 3;

    data.reserve(data.size() + vertexCount * 10);
    for (size_t i = 0; i < vertexCount; ++i) {
        data.push_back(vertices[i * 3 + 0]);
        data.push_back(vertices[i * 3 + 1]);
        data.push_back(vertices[i * 3 + 2]);
        data.push_back(color.r);
        data.push_back(color.g);
        data.push_back(color.b);
        data.push_back(1.0f);
        data.push_back(normals[i * 3 + 0]);
        data.push_back(normals[i * 3 + 1]);
        data.push_back(normals[i * 3 + 2]);
    }
}

AABB Plane::getBounds() const
{
    return {};
}
