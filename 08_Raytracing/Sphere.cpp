#include "Sphere.h"

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
	Vec3 l = center - ray.getOrigin();

	float tCenter = Vec3::dot(l, ray.getDirection());
	if (tCenter < 0)
		return {};	// no intersection

	float dSq = l.sqlength() - tCenter * tCenter;
	if (dSq > sqradius)
		return {};	// no intersection

	float dist = sqrt(sqradius - dSq);
	float t = tCenter - dist;

	if (t < 0)
		t = tCenter + dist;	// when inside sphere

	Vec3 normal = ray.getPosOnRay(t) - center;
	normal = Vec3::normalize(normal);

	return Intersection{ material, normal, t };
}
