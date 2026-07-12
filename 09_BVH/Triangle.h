#pragma once

#include "IntersectableObject.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

class Triangle : public IntersectableObject {
private:
    const Vec3 a;
    const Vec3 b;
    const Vec3 c;
    const Vec3 normal;
    const std::array<Vec3, 3> vertexNormals;
    const Material material;

public:
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Material& material);
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c,
             const Vec3& normalA, const Vec3& normalB, const Vec3& normalC,
             const Material& material);
    virtual ~Triangle() {}

    Material getMaterial() const override;
    std::optional<Intersection> intersect(const Ray& ray) const override;
    AABB getBounds() const override;
    void appendTriangleData(std::vector<float>& data) const override;

    static std::vector<std::shared_ptr<const IntersectableObject>> loadOBJ(const std::string& filename,
                                                                           const Material& material,
                                                                           const Vec3& scale = Vec3{1.0f},
                                                                           const Vec3& translation = Vec3{0.0f},
                                                                           bool normalize = true);
};
