#pragma once
#include "IntersectableObject.h"

class Sphere : public IntersectableObject
{
private:
	const Vec3 center;
	const double sqradius;
	const Material material;

public:
	Sphere(const Vec3& center, double radius, const Material& material);
    virtual ~Sphere() {}

	Material getMaterial() const override;
	std::optional<Intersection> intersect(const Ray& ray) const override;

};

