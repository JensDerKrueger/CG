#pragma once
#include "IntersectableObject.h"

class Sphere : public IntersectableObject
{

public:
	Sphere(const Vec3& center, float radius, const Material& material);
    virtual ~Sphere() {}
    
	Material getMaterial() const override;
	std::optional<Intersection> intersect(const Ray& ray) const override;

};

