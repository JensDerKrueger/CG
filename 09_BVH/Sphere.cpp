#include "Sphere.h"
#include <Tessellation.h>
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

void Sphere::appendTriangleData(std::vector<float>& data) const
{
	const Tessellation mesh = Tessellation::genSphere(center, std::sqrt(sqradius), 32, 16).unpack();
	const std::vector<float>& vertices = mesh.getVertices();
	const std::vector<float>& normals = mesh.getNormals();
	const Vec3 color = material.getDiffuse();
	const size_t vertexCount = vertices.size() / 3;

	data.reserve(data.size() + vertexCount * 10);
	for (size_t i = 0; i < vertexCount; ++i) {
		data.push_back(vertices[i * 3 + 0]);
		data.push_back(vertices[i * 3 + 1]);
		data.push_back(vertices[i * 3 + 2]);
		data.push_back(color.r);
		data.push_back(color.g);
		data.push_back(color.b);
		data.push_back(1.0f);
		data.push_back(normals[i * 3 + 0]);
		data.push_back(normals[i * 3 + 1]);
		data.push_back(normals[i * 3 + 2]);
	}
}

AABB Sphere::getBounds() const
{
	const float radius = std::sqrt(sqradius);
	const Vec3 extent{radius, radius, radius};
	return AABB{center - extent, center + extent};
}
