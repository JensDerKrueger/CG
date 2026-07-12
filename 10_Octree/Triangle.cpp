#include "Triangle.h"

#include <OBJFile.h>
#include <array>
#include <cmath>

namespace {

constexpr float TRIANGLE_EPSILON = 0.000001f;

} // namespace

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Material& material)
    : a(a),
      b(b),
      c(c),
      normal(Vec3::normalize(Vec3::cross(b - a, c - a))),
      vertexNormals{normal, normal, normal},
      material(material)
{}

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c,
                   const Vec3& normalA, const Vec3& normalB, const Vec3& normalC,
                   const Material& material)
    : a(a),
      b(b),
      c(c),
      normal(Vec3::normalize(Vec3::cross(b - a, c - a))),
      vertexNormals{Vec3::normalize(normalA), Vec3::normalize(normalB), Vec3::normalize(normalC)},
      material(material)
{}

Material Triangle::getMaterial() const {
    return material;
}

std::optional<Intersection> Triangle::intersect(const Ray& ray) const {
    const Vec3 edgeAB = b - a;
    const Vec3 edgeAC = c - a;
    const Vec3 p = Vec3::cross(ray.getDirection(), edgeAC);
    const float determinant = Vec3::dot(edgeAB, p);

    if (std::fabs(determinant) < TRIANGLE_EPSILON)
        return {};

    const float inverseDeterminant = 1.0f / determinant;
    const Vec3 originToA = ray.getOrigin() - a;
    const float u = Vec3::dot(originToA, p) * inverseDeterminant;
    if (u < 0.0f || u > 1.0f)
        return {};

    const Vec3 q = Vec3::cross(originToA, edgeAB);
    const float v = Vec3::dot(ray.getDirection(), q) * inverseDeterminant;
    if (v < 0.0f || u + v > 1.0f)
        return {};

    const float t = Vec3::dot(edgeAC, q) * inverseDeterminant;
    if (t < TRIANGLE_EPSILON)
        return {};

    const float w = 1.0f - u - v;
    Vec3 interpolatedNormal = vertexNormals[0] * w + vertexNormals[1] * u + vertexNormals[2] * v;
    if (interpolatedNormal.sqlength() == 0.0f)
        interpolatedNormal = normal;
    else
        interpolatedNormal = Vec3::normalize(interpolatedNormal);

    const Vec3 orientedNormal = Vec3::dot(ray.getDirection(), interpolatedNormal) > 0.0f ? interpolatedNormal * -1.0f : interpolatedNormal;
    return Intersection{material, orientedNormal, t};
}

AABB Triangle::getBounds() const {
    return AABB::fromPoints(a, b, c);
}

void Triangle::appendTriangleData(std::vector<float>& data) const {
    const Vec3 color = material.getDiffuse();
    const std::array<Vec3, 3> vertices{a, b, c};

    data.reserve(data.size() + 30);
    for (size_t i = 0; i < vertices.size(); ++i) {
        const Vec3& vertex = vertices[i];
        const Vec3& vertexNormal = vertexNormals[i];
        data.push_back(vertex.x);
        data.push_back(vertex.y);
        data.push_back(vertex.z);
        data.push_back(color.r);
        data.push_back(color.g);
        data.push_back(color.b);
        data.push_back(1.0f);
        data.push_back(vertexNormal.x);
        data.push_back(vertexNormal.y);
        data.push_back(vertexNormal.z);
    }
}

std::vector<std::shared_ptr<const IntersectableObject>> Triangle::loadOBJ(const std::string& filename,
                                                                          const Material& material,
                                                                          const Vec3& scale,
                                                                          const Vec3& translation,
                                                                          bool normalize) {
    const OBJFile obj{filename, normalize};
    std::vector<std::shared_ptr<const IntersectableObject>> triangles;
    triangles.reserve(obj.indices.size());

    for (const OBJFile::IndexType& index : obj.indices) {
        const Vec3 a = obj.vertices[index[0]] * scale + translation;
        const Vec3 b = obj.vertices[index[1]] * scale + translation;
        const Vec3 c = obj.vertices[index[2]] * scale + translation;
        const Vec3 normalA = obj.normals[index[0]];
        const Vec3 normalB = obj.normals[index[1]];
        const Vec3 normalC = obj.normals[index[2]];
        triangles.push_back(std::make_shared<Triangle>(a, b, c, normalA, normalB, normalC, material));
    }

    return triangles;
}
