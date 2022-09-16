#pragma once
#include "IntersectableObject.h"

class Sphere : public IntersectableObject
{
private:
	const Vec3 center;
	const double sqradius;
	const Material material;
	Vec3 rotation;
	TextureCoordinates scale;
	TextureCoordinates bias;

public:
	Sphere(const Vec3& center, double radius, const Material& material);
	Sphere(const Vec3& center, double radius, const Material& material, const Vec3& rotation);
	Sphere(const Vec3& center, double radius, const Material& material, const Vec3& rotation,
		   const TextureCoordinates& scale, const TextureCoordinates& bias);

    virtual ~Sphere() {}

	Material getMaterial() const override;
	std::optional<Intersection> intersect(const Ray& ray) const override;

};

