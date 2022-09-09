#pragma once
#include "Material.h"
#include "Intersection.h"
#include "Ray.h"

#include <optional>

class IntersectableObject
{
public:
	virtual Material getMaterial() const = 0;
	virtual std::optional<Intersection> intersect(const Ray& ray) const = 0;

};

