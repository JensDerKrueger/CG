#pragma once
#include "IntersectableObject.h"
class Plane : public IntersectableObject
{
private:
	const Vec3 normal;
	const float d;
	const Material material;

public:
	Plane(const Vec3& normal, float d, const Material& material)
		: normal(normal), d(d), material(material)
	{ }

    virtual ~Plane() {}
    
	Material getMaterial() const override;
	std::optional<Intersection> intersect(const Ray& ray) const override;
	AABB getBounds() const override;
	void appendTriangleData(std::vector<float>& data) const override;
};
