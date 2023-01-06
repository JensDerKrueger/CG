#include "Scene.h"
#include "Material.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Plane.h"

#include <iostream>

#include "Texture.h"

void Scene::addObject(std::shared_ptr<const IntersectableObject> object) {
	sceneObjects.push_back(object);
}

void Scene::addLight(std::shared_ptr<const LightSource> ls) {
	lightSources.push_back(ls);
}

Vec3 Scene::getBackgroundcolor() const {
	return backgroundColor;
}

std::optional<Intersection> Scene::intersect(const Ray& ray,
                                             bool shadowRay) const {
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
Vec3 Scene::traceRay(const Ray& ray, float IOR, int recDepth) const {
	if (recDepth == 0) return backgroundColor;

	// no intersection found
	std::optional<Intersection> opt_intersection = intersect(ray, false);
	if (!opt_intersection.has_value()) return backgroundColor;

	// else intersection found, do recursive ray tracing
	Intersection inter = opt_intersection.value();
	Vec3 interPos = ray.getPosOnRay(inter.getT());

	if (debug) {
		std::optional<TextureCoordinates> tc = inter.getTexCoords();
		if (tc.has_value()) {
			return Vec3::clamp({ tc->u, tc->v, 0 }, 0, 1);
		}
	}

	Vec3 offSurfacePos = interPos + inter.getNormal() * OFFSET_EPSILON;

	Vec3 reflColor{ 0.0f, 0.0f, 0.0f };
	if (inter.getMaterial().reflects()) {
		Ray reflRay{ offSurfacePos, Vec3::reflect(ray.getDirection(), inter.getNormal()) };
		reflColor = traceRay(reflRay, IOR, recDepth - 1);
	}

	Vec3 refractionColor{ 0.0f, 0.0f, 0.0f };
	if (inter.getMaterial().refracts()) {
		std::optional<Vec3> refrDirection = Vec3::refract(ray.getDirection(), inter.getNormal(), inter.getMaterial().getIndexOfRefraction().value());
		if (refrDirection.has_value()) {
			if (IOR == 1.0) {
				// Ray --> from air into material
				Vec3 inSurfacePos = interPos + inter.getNormal() * -OFFSET_EPSILON;
				Ray refrRay{ inSurfacePos, refrDirection.value() };
				refractionColor = traceRay(refrRay, inter.getMaterial().getIndexOfRefraction().value(), recDepth - 1);
			} else {
				// Ray --> from material into air
				Ray refrRay{ offSurfacePos, refrDirection.value() };
				refractionColor = traceRay(refrRay, 1.0, recDepth - 1);
			}
		}
	}

	Vec3 localColor{ 0.0f, 0.0f, 0.0f };
	Vec3 textureColor{1.0f, 1.0f, 1.0f}; // multiplication with this color results in same color value
	if(inter.getMaterial().hasTexture() && inter.getTexCoords().has_value()) {
		textureColor = inter.getMaterial().getTexture().value().sample(inter.getTexCoords().value());
	}

	for (const std::shared_ptr<const LightSource>& ls : lightSources) {
		Ray shadowRay{ offSurfacePos, ls->getDirection(offSurfacePos) };
		std::optional<Intersection> shadowInter = intersect(shadowRay, true);

		Vec3 ambient = inter.getMaterial().getAmbient() * ls->getAmbient();
		if (inter.getMaterial().hasTexture()) {
			ambient = ambient * textureColor;
		}

		if (!shadowInter.has_value() || shadowInter->getT() > ls->getDistance(offSurfacePos)) {
			float d = Vec3::dot(ls->getDirection(offSurfacePos), inter.getNormal());
			Vec3 diffuse = inter.getMaterial().getDiffuse() * ls->getDiffuse() * d;
			diffuse = Vec3::clamp(diffuse, 0.0f, 1.0f);
			if (inter.getMaterial().hasTexture()) {
				diffuse = diffuse * textureColor;
			}

			Vec3 Rv = Vec3::reflect(ray.getDirection(), inter.getNormal());
			float s = pow(std::max(0.0f, Vec3::dot(Rv, ls->getDirection(offSurfacePos))), inter.getMaterial().getExp());
			Vec3 specular = inter.getMaterial().getSpecular() * ls->getSpecular() * s;
			specular = Vec3::clamp(specular, 0.0f, 1.0f);

			localColor = localColor + ambient + diffuse + specular;
		} else {
			localColor = localColor + ambient;
		}
	}

	// compose final color
	float cosI = Vec3::dot(ray.getDirection(), inter.getNormal());
	float l = 0, r = 0, t = 0;
	if (inter.getMaterial().refracts()) {
		l = inter.getMaterial().getLocalRefectivity();
		r = inter.getMaterial().getReflectivity(cosI);
		t = 1 - r;
		r = (1 - l) * r;
		t = (1 - l) * t;
	} else if (inter.getMaterial().reflects()) {
		r = inter.getMaterial().getReflectivity(cosI);
		l = 1 - r;
	} else {
		l = 1;
	}
	return localColor * l + reflColor * r + refractionColor * t;
}

Scene Scene::genTexturedScene() {
	// create an empty scene
	Scene s;

	Texture checkerboard = Texture::genCheckerboardTexture(2, 2);

	Texture earth("Textures/Earth.png");
	Texture hpcLight("Textures/HPC-Light.png");
	Texture hpcDark("Textures/HPC-Dark.png");

	hpcLight.setBorderMode(BorderMode::CLAMP_TO_EDGE);
	hpcDark.setBorderModeU(BorderMode::MIRRORED_REPEAT);
	hpcDark.setBorderModeV(BorderMode::CLAMP_TO_BORDER);
	hpcDark.setBorderColor(Vec3{ 1.0f, 1.0f, 1.0f });

	// and God said, let there be light and there was light
	std::shared_ptr<const PointLight> l = std::make_shared<const PointLight>(Vec3{ 0, 4, -2 }, Vec3{ 1, 1, 1 }, Vec3{ 1, 1, 1 }, Vec3{ 1, 1, 1 });

	// attach the light source to the scene
	s.addLight(l);

	// create the redish material for the right sphere
	// vec3 are treated as color values in the range [0, 1]
	Material m(Vec3(0.9f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), 20, 0.2f, 1.52f, hpcDark);

	// create a sphere, apply the material above to it and attach it to the scene
	float i = 6;
	TextureCoordinates scale{ 2.0f * i, i };
	TextureCoordinates bias{ (1 - scale.u) / 2, (1 - scale.v) / 2 };
	s.addObject(std::make_shared<Sphere>(Vec3{ 0.7f, -0.4f, -2.0f }, 0.9f, m, Vec3{-75, -25, -60}, scale, bias));

	// create the earth material and apply it to the left sphere
	m = Material(Vec3{ 1.0f, 1.0f, 1.0f }, Vec3{ 1.0f, 1.0f, 1.0f }, Vec3{ 0.2f, 0.2f, 0.2f }, 8, 1, {}, earth);
	scale = TextureCoordinates{ 1.0f, 1.0f };
	bias = TextureCoordinates{ 0.0f, 0.0f };
	s.addObject(std::make_shared<Sphere>(Vec3{ -0.9f, -0.1f, -2.2f }, 0.6f, m, Vec3{ -90, 0, -90 }, scale, bias));

	// create the yellowish material and apply it to the big sphere in the back
	m = Material(Vec3{ 0.3f, 0.3f, 0.0f }, Vec3{ 0.7f, 0.7f, 0.0f }, Vec3{ 1.0f, 1.0f, 0.0f }, 8, 0.3f, {}, hpcLight);

	i = 4;
	scale = TextureCoordinates{ 2.0f * i, i };
	bias = TextureCoordinates{ (1 - scale.u) / 2, (1 - scale.v) / 2 };
	s.addObject(std::make_shared<Sphere>(Vec3{ 0.0f, 4.0f, -8.0f }, 3.9f, m, Vec3{-60, 0, -90}, scale, bias));

	// create the ground plane with the checkerboard texture
	m = Material(Vec3{ 0.3f, 0.3f, 0.3f }, Vec3{ 0.5f, 0.5f, 0.5f }, Vec3{ 1.0f, 1.0f, 1.0f }, 32, 0.5f, {}, checkerboard);
	s.addObject(std::make_shared<Plane>(Vec3{ 0.0f, 1.0f, 0.0f }, 1.5f, m));

	return s;
}

void Scene::setDebug(bool debug) {
	this->debug = debug;
}
