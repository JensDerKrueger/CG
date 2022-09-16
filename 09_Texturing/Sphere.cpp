#include "Sphere.h"
#include <Mat3.h>

#ifndef M_PI
constexpr float M_PI = 3.14159265358979323846f;
#endif

Sphere::Sphere(const Vec3& center, double radius, const Material& material)
	: Sphere(center, radius, material, Vec3{ 0, 0, 0 }, TextureCoordinates{ 1.0f, 1.0f }, TextureCoordinates{0.0f, 0.0f})
{

}

Sphere::Sphere(const Vec3& center, double radius, const Material& material, const Vec3& rotation)
	: Sphere(center, radius, material, rotation, TextureCoordinates{ 1.0f, 1.0f }, TextureCoordinates{ 0.0f, 0.0f })
{

}

Sphere::Sphere(const Vec3& center, double radius, const Material& material, const Vec3& rotation, const TextureCoordinates& scale, const TextureCoordinates& bias)
	: center(center), sqradius(radius*radius), material(material), rotation(rotation), scale(scale), bias(bias)
{

}

Material Sphere::getMaterial() const
{
	return material;
}

std::optional<Intersection> Sphere::intersect(const Ray& ray) const
{
	Vec3 l = center - ray.getOrigin();

	double tCenter = Vec3::dot(l, ray.getDirection());
	if (tCenter < 0)
		return {};	// no intersection

	double dSq = l.sqlength() - tCenter * tCenter;
	if (dSq > sqradius)
		return {};	// no intersection

	double dist = sqrt(sqradius - dSq);
	double t = tCenter - dist;

	if (t < 0)
		t = tCenter + dist;	// when inside sphere

	Vec3 normal = ray.getPosOnRay(t) - center;
	normal = Vec3::normalize(normal);

	std::optional<TextureCoordinates> tc{};
	if (material.hasTexture())
	{
		Vec3 r = normal;
		// TODO Task01: Complete texture coordinate computation for a sphere
		// The sphere has a rotation in Euler angles. This rotation should be considered when applying textures.
		// First rotate the radial coordinate vector r by applying a right handed rotation.
		// Then compute the normalized texture coordinates u,v.
		// In a last step scale and offset the coordinates by the given values.

	}

	return Intersection{ material, normal, tc, t };
}
