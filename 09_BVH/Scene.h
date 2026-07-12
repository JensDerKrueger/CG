#pragma once
#include <cstdint>
#include <memory>
#include <Mat4.h>
#include <optional>
#include <Vec3.h>
#include <vector>
#include "AABB.h"
#include "IntersectableObject.h"
#include "LightSource.h"

class Scene
{
private:
	struct BVHNode {
		AABB bounds;
		uint32_t left{0};
		uint32_t right{0};
		uint32_t firstObject{0};
		uint32_t objectCount{0};

		bool isLeaf() const { return objectCount > 0; }
	};

	static constexpr float OFFSET_EPSILON = 0.00001f;
	static constexpr uint32_t BVH_LEAF_SIZE = 4;
	std::vector<std::shared_ptr<const IntersectableObject>> sceneObjects;
	std::vector<std::shared_ptr<const LightSource>> lightSources;
	std::vector<size_t> bvhObjectIndices;
	std::vector<BVHNode> bvhNodes;
	Vec3 backgroundColor;
	Mat4 model;

	uint32_t buildBVHNode(uint32_t begin, uint32_t end);
	std::optional<Intersection> intersectBruteForce(const Ray& ray, bool shadowRay) const;
	std::optional<Intersection> intersectBVH(const Ray& ray, bool shadowRay) const;
	std::optional<Intersection> intersectBVHNode(uint32_t nodeIndex, const Ray& ray, bool shadowRay, float maxT) const;
	Vec3 traceLocalRay(const Ray& ray, float IOR, int recDepth, bool useBVH) const;

public:
	Scene()
		: Scene(Vec3{0.2f, 0.2f, 0.2f})
	{}

	Scene(const Vec3& backgroundColor)
		: backgroundColor(backgroundColor)
	{ }

	void addObject(std::shared_ptr<const IntersectableObject> object);
	void addLight(std::shared_ptr<const LightSource> ls);
	void buildBVH();
	std::shared_ptr<const LightSource> getLight(size_t index) const;
	void setModel(const Mat4& model);
	Mat4 getModel() const;
	Vec3 getBackgroundcolor() const;
	std::vector<float> getTriangleData() const;
	std::vector<float> getBVHLineData(uint32_t maxDepth) const;
	std::optional<Intersection> intersect(const Ray& ray, bool shadowRay, bool useBVH) const;
	Vec3 traceRay(const Ray& ray, float IOR, int recDepth, bool useBVH) const;

	static Scene genSimpleScene();

};
