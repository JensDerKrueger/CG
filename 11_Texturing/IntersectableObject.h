#pragma once
#include "Material.h"
#include "Intersection.h"
#include "Ray.h"
#include "Triangulate.h"

#include <optional>

class IntersectableObject
	: public Triangulate
{
public:
	virtual ~IntersectableObject() = default;
	virtual Material getMaterial() const = 0;
	virtual std::optional<Intersection> intersect(const Ray& ray) const = 0;

};

