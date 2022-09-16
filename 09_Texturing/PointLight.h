#pragma once
#include "LightSource.h"
#include <Vec3.h>

class PointLight : public LightSource
{
protected:
	Vec3 position;

public:
	PointLight(const Vec3& position, const Vec3& ambient, const Vec3& diffuse, const Vec3& specular)
		: LightSource(ambient, diffuse, specular), position(position)
	{ }

  virtual ~PointLight() {}

	//Returns normalized direction from position to this PointLight
	virtual Vec3 getDirection(const Vec3& position) const override;

	// Returns distance from position to this PointLight
	virtual float getDistance(const Vec3& position) const override;
};

