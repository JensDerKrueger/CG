#pragma once
#include <Vec3.h>

class Ray
{
private:
	Vec3 origin;
	Vec3 direction;

public:
	Ray(const Vec3& origin, const Vec3& direction)
		: origin(origin), direction(direction)
	{ }

	Vec3 getOrigin() const;
	Vec3 getDirection() const;
	Vec3 getPosOnRay(float t) const;
};

