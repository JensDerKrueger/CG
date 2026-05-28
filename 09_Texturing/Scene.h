#pragma once
#include <memory>
#include <Mat4.h>
#include <optional>
#include <Vec3.h>
#include <vector>
#include "IntersectableObject.h"
#include "LightSource.h"

class Scene
{
	static constexpr float OFFSET_EPSILON = 0.00001f;
	std::vector<std::shared_ptr<const IntersectableObject>> sceneObjects;
	std::vector<std::shared_ptr<const LightSource>> lightSources;
	Vec3 backgroundColor;
	Mat4 model;
	bool debug;

	Vec3 traceLocalRay(const Ray& ray, float IOR, int recDepth) const;

public:
	Scene()
		: Scene(Vec3{0.2f, 0.2f, 0.2f})
	{}

	Scene(const Vec3& backgroundColor)
		: backgroundColor(backgroundColor), debug(false)
	{ }

	void addObject(std::shared_ptr<const IntersectableObject> object);
	void addLight(std::shared_ptr<const LightSource> ls);
	std::shared_ptr<const LightSource> getLight(size_t index) const;
	void setModel(const Mat4& model);
	Mat4 getModel() const;
	Vec3 getBackgroundcolor() const;
	std::vector<float> getTriangleData() const;
	std::optional<Intersection> intersect(const Ray& ray, bool shadowRay) const;
	Vec3 traceRay(const Ray& ray, float IOR, int recDepth) const;

	static Scene genTexturedScene();

	void setDebug(bool debug);

};
