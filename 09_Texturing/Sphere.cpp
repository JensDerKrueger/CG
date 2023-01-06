#include "Sphere.h"
#include <Mat3.h>

#ifndef M_PI
constexpr float M_PI = 3.14159265358979323846f;
#endif

Sphere::Sphere(const Vec3& center, float radius, const Material& material) :
Sphere(center, radius, material, { 0, 0, 0 }, { 1.0f, 1.0f }, {0.0f, 0.0f})
{
}

Sphere::Sphere(const Vec3& center, float radius, const Material& material,
               const Vec3& rotation) :
Sphere(center, radius, material, rotation, { 1.0f, 1.0f }, { 0.0f, 0.0f })
{
}

Sphere::Sphere(const Vec3& center, float radius, const Material& material,
               const Vec3& rotation, const TextureCoordinates& scale,
               const TextureCoordinates& bias) :
center(center),
sqradius(radius*radius),
material(material),
rotation(rotation),
scale(scale),
bias(bias)
{
}

Material Sphere::getMaterial() const {
  return material;
}

std::optional<Intersection> Sphere::intersect(const Ray& ray) const {
  const Vec3 l = center - ray.getOrigin();
  const float tCenter = Vec3::dot(l, ray.getDirection());
  if (tCenter < 0) return {};  // no intersection

  const float dSq = l.sqlength() - tCenter * tCenter;
  if (dSq > sqradius) return {};  // no intersection

  const float dist = sqrt(sqradius - dSq);
  float t = tCenter - dist;

  if (t < 0) t = tCenter + dist;  // when inside sphere

  const Vec3 normal = Vec3::normalize(ray.getPosOnRay(t) - center);
  if (!material.hasTexture()) return Intersection{material, normal, {}, t };

  Vec3 r = normal;
  // TODO Task01: Complete texture coordinate computation for a sphere
  // The sphere has a rotation in Euler angles. This rotation should be considered when applying textures.
  // First rotate the radial coordinate vector r by applying a right handed rotation.
  // Then compute the normalized texture coordinates u,v.
  // In a last step scale and offset the coordinates by the given values.

  return Intersection{material, normal, {}, t };
}
