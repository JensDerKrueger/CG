#pragma once
#include "Material.h"
#include <Vec3.h>
#include "TextureCoordinates.h"

class Intersection
{
private:
	Material material;
	Vec3 normal;
	std::optional<TextureCoordinates> texCoords;
	float t;

public:
	Intersection(const Material& material, const Vec3& normal, const std::optional<TextureCoordinates>& texCoords, float t)
		: material(material), normal(normal), texCoords(texCoords), t(t)
	{ };

	Material getMaterial() const;
	Vec3 getNormal() const;
	float getT() const;
	std::optional<TextureCoordinates> getTexCoords() const;

};

