#pragma once
#include <Vec3.h>

class LightSource
{
private:
	Vec3 ambient;
	Vec3 diffuse;
	Vec3 specular;

public:
	LightSource(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular)
		: ambient(ambient), diffuse(diffuse), specular(specular)
	{ }

	Vec3 getAmbient() const { return ambient; }

	Vec3 getDiffuse() const { return diffuse; }

	Vec3 getSpecular() const { return specular; }

	virtual Vec3 getDirection(const Vec3& position) const = 0;
	virtual double getDistance(const Vec3& position) const = 0;

};

