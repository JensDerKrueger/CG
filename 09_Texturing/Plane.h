#pragma once
#include "IntersectableObject.h"
class Plane : public IntersectableObject
{
	const Vec3 normal;
	const double d;
	const Material material;
	Vec3 frame1;
	Vec3 frame2;
	Vec3 center;

public:
	Plane(const Vec3& normal, double d, const Material& material)
		: normal(normal), d(d), material(material)
	{ 
		buildLocalFrame();
	}

    virtual ~Plane() {}
    
	Material getMaterial() const override;
	std::optional<Intersection> intersect(const Ray& ray) const override;

private:
	void buildLocalFrame();
};

