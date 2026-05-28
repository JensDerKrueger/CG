#include "Sphere.h"

#include <cmath>

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
	: center(center), sqradius(radius* radius), material(material)
{
}

Material Sphere::getMaterial() const
{
	return material;
}

std::optional<Intersection> Sphere::intersect(const Ray& ray) const
{
  // TODO: implement this method

  // just a dummy return value, needs to be replaced with the right one
  return {};
}

Tessellation Sphere::getMesh() const
{
	return Tessellation::genSphere(center, std::sqrt(sqradius), 32, 16);
}
