#include "Scene.h"
#include "Material.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Plane.h"

namespace {

Vec3 transformDirection(const Mat4& matrix, const Vec3& direction)
{
	return (matrix * Vec4{direction, 0.0f}).xyz;
}

} // namespace

void Scene::addObject(std::shared_ptr<const IntersectableObject> object)
{
	sceneObjects.push_back(object);
}

void Scene::addLight(std::shared_ptr<const LightSource> ls)
{
	lightSources.push_back(ls);
}

std::shared_ptr<const LightSource> Scene::getLight(size_t index) const
{
  if (index >= lightSources.size())
    return {};

  return lightSources[index];
}

void Scene::setModel(const Mat4& model)
{
	// This local-space raytracing path assumes translations, rotations, and uniform scales only.
	this->model = model;
}

Mat4 Scene::getModel() const
{
	return model;
}

Vec3 Scene::getBackgroundcolor() const
{
	return backgroundColor;
}

std::vector<float> Scene::getTriangleData() const
{
	std::vector<float> data;

	for (std::shared_ptr<const IntersectableObject> object : sceneObjects)
	{
		const Tessellation mesh = object->getMesh().unpack();
		const std::vector<float>& vertices = mesh.getVertices();
		const std::vector<float>& normals = mesh.getNormals();
		const Vec3 color = object->getMaterial().getDiffuse();
		const size_t vertexCount = vertices.size() / 3;

		data.reserve(data.size() + vertexCount * 10);
		for (size_t i = 0; i < vertexCount; ++i)
		{
			data.push_back(vertices[i * 3 + 0]);
			data.push_back(vertices[i * 3 + 1]);
			data.push_back(vertices[i * 3 + 2]);
			data.push_back(color.r);
			data.push_back(color.g);
			data.push_back(color.b);
			data.push_back(1.0f);

			if (normals.size() >= (i + 1) * 3)
			{
				data.push_back(normals[i * 3 + 0]);
				data.push_back(normals[i * 3 + 1]);
				data.push_back(normals[i * 3 + 2]);
			}
			else
			{
				data.push_back(0.0f);
				data.push_back(0.0f);
				data.push_back(1.0f);
			}
		}
	}

	return data;
}

std::optional<Intersection> Scene::intersect(const Ray& ray, bool shadowRay) const
{
	std::optional<Intersection> result{};
	for (std::shared_ptr<const IntersectableObject> object : sceneObjects)
	{
		if (shadowRay && !object->getMaterial().isShadowCaster())
			continue;

		std::optional<Intersection> i = object->intersect(ray);
		if (!i.has_value())
			continue;

		if (!result.has_value() || i.value().getT() < result.value().getT())
			result = i;
	}
	return result;
}

/// <summary>
/// Trace a ray through the scene and compute its color value.
/// </summary>
/// <param name="ray">to trace</param>
/// <param name="IOR">optical density of the material we are currently travelling in</param>
/// <param name="recDepth">recursion depth</param>
/// <returns>final color value computed for this ray</returns>
Vec3 Scene::traceRay(const Ray& ray, float IOR, int recDepth) const
{
  // TODO: implement the missing parts of this method according to the exercise

	const Mat4 inverseModel = Mat4::inverse(model);
	const Vec3 localDirection = Vec3::normalize(transformDirection(inverseModel, ray.getDirection()));
	if (localDirection.sqlength() == 0.0f)
		return backgroundColor;

	const Ray localRay{ inverseModel * ray.getOrigin(), localDirection };

	// no intersection found
	std::optional<Intersection> opt_intersection = intersect(localRay, false);
	if (!opt_intersection.has_value())
		return backgroundColor;

	// else intersection found
	Intersection inter = opt_intersection.value();
	Vec3 interPos = localRay.getPosOnRay(inter.getT());

	Vec3 localColor;
	for (std::shared_ptr<const LightSource> ls : lightSources)
	{
		const Vec3 localLightDirection = ls->getDirection(interPos);

		Vec3 ambient = inter.getMaterial().getAmbient() * ls->getAmbient();

			float d = Vec3::dot(localLightDirection, inter.getNormal());
			Vec3 diffuse = inter.getMaterial().getDiffuse() * ls->getDiffuse() * d;
			diffuse = Vec3::clamp(diffuse, 0.0f, 1.0f);

			Vec3 Rv = Vec3::reflect(localRay.getDirection(), inter.getNormal());
			float s = pow(std::max(0.0f, Vec3::dot(Rv, localLightDirection)), inter.getMaterial().getExp());
			Vec3 specular = inter.getMaterial().getSpecular() * ls->getSpecular() * s;
			specular = Vec3::clamp(specular, 0.0f, 1.0f);

			localColor = localColor + ambient + diffuse + specular;
	}

	return localColor;
}

Scene Scene::genSimpleScene()
{
	// create an empty scene
	Scene s;

	// and God said, let there be light and there was light
	std::shared_ptr<const PointLight> l = std::make_shared<const PointLight>(Vec3{ 0, 4, -2 }, Vec3{ 1, 1, 1 }, Vec3{ 1, 1, 1 }, Vec3{ 1, 1, 1 });

	// attach the light source to the scene
	s.addLight(l);

	// create the bluish material for the right sphere
	// vec3 are treated as color values in the range [0, 1]
	Material m(Vec3(0.0f, 0.0f, 0.3f), Vec3(0.0f, 0.0f, 0.5f), Vec3(1.0f, 1.0f, 1.0f), 8, 0.2, 1.52);

	// create a sphere, apply the material above to it and attach it to the scene
	s.addObject(std::make_shared<Sphere>(Vec3{ 0.7f, -0.4f, -2.0f }, 0.9, m));

	// create the red material and apply it to the left sphere
	m = Material(Vec3{ 0.3f, 0.0f, 0.0f }, Vec3{ 0.5f, 0.0f, 0.0f }, Vec3{ 1.0f, 1.0f, 1.0f }, 8, 1);
	s.addObject(std::make_shared<Sphere>(Vec3{ -0.9f, -0.1f, -2.2f }, 0.6, m));

	// create the yellowish material and apply it to the big sphere in the back
	m = Material(Vec3{ 0.3f, 0.3f, 0.0f }, Vec3{ 0.7f, 0.7f, 0.0f }, Vec3{ 1.0f, 1.0f, 0.0f }, 8, 0.3);
	s.addObject(std::make_shared<Sphere>(Vec3{ 0, 4, -8 }, 3.9, m));

	// create the white ground plane
	m = Material(Vec3{ 0.3f, 0.3f, 0.3f }, Vec3{ 0.5f, 0.5f, 0.5f }, Vec3{ 1.0f, 1.0f, 1.0f }, 32, 0.5);
	s.addObject(std::make_shared<Plane>(Vec3{ 0.0f, 1.0f, 0.0f }, 1.5, m));

	return s;
}
