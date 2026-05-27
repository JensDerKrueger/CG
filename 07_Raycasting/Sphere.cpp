#include "Sphere.h"

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
{
	// TODO: implement this constructor and the rest of this class necessary for the assignment
}

Material Sphere::getMaterial() const
{
	// TODO: implement this method
	return Material{ Vec3{}, Vec3{}, Vec3{}, 0 }; // just a dummy return value, needs to be replaced with the right one
}

std::optional<Intersection> Sphere::intersect(const Ray& ray) const
{
	// TODO: implement this method
	return {}; // just a dummy return value, needs to be replaced with the right one
}
