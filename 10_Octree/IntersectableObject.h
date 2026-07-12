#pragma once
#include "AABB.h"
#include "Material.h"
#include "Intersection.h"
#include "Ray.h"

#include <optional>
#include <vector>

class IntersectableObject
{
public:
	virtual ~IntersectableObject() = default;
	virtual Material getMaterial() const = 0;
	virtual std::optional<Intersection> intersect(const Ray& ray) const = 0;
	virtual AABB getBounds() const = 0;
	virtual void appendTriangleData(std::vector<float>& data) const = 0;

};

