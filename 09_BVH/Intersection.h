#pragma once
#include "Material.h"
#include <Vec3.h>

class Intersection
{
private:
	Material material;
	Vec3 normal;
	float t;

public:
	Intersection(const Material& material, const Vec3& normal, float t)
		: material(material), normal(normal), t(t)
	{ };

	Material getMaterial() const;
	Vec3 getNormal() const;
	float getT() const;

};

