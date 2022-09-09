#pragma once
#include "Material.h"

// TODO: implement the rest of this class necessary for the assignment
class Vertex
{
public:
	Vertex(const Vec3& position, const Material& material, const Vec3& normal)
	{ }

	Vertex(const Vec3& position, const Material& material)
		: Vertex(position, material, Vec3{0, 0, 1})
	{ }

	Vertex(const Vec3& position)
		: Vertex(position, Material{Vec3{1,1,1}})
	{ }

	Vertex()
		: Vertex(Vec3{0, 0, 0})
	{ }


};

