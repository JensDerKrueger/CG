#pragma once
#include <cstdint>
#include <array>
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
	static constexpr uint32_t INVALID_NODE = UINT32_MAX;

	struct OctreeNode {
		AABB bounds;
		std::array<uint32_t, 8> children{};
		uint32_t firstObject{0};
		uint32_t objectCount{0};

		OctreeNode() {
			children.fill(INVALID_NODE);
		}

		bool isLeaf() const {
			for (uint32_t child : children) {
				if (child != INVALID_NODE)
					return false;
			}
			return true;
		}
	};

	static constexpr float OFFSET_EPSILON = 0.00001f;
	static constexpr uint32_t OCTREE_LEAF_SIZE = 8;
	static constexpr uint32_t OCTREE_MAX_DEPTH = 8;
	static constexpr uint32_t OCTREE_MAX_CHILD_REFERENCES = 4;
	std::vector<std::shared_ptr<const IntersectableObject>> sceneObjects;
	std::vector<std::shared_ptr<const LightSource>> lightSources;
	std::vector<size_t> octreeObjectIndices;
	std::vector<OctreeNode> octreeNodes;
	mutable std::vector<uint32_t> octreeMailbox;
	mutable uint32_t octreeMailboxStamp{0};
	Vec3 backgroundColor;
	Mat4 model;

	uint32_t buildOctreeNode(const AABB& bounds, const std::vector<size_t>& objectIndices, uint32_t depth);
	std::optional<Intersection> intersectBruteForce(const Ray& ray, bool shadowRay) const;
	std::optional<Intersection> intersectOctree(const Ray& ray, bool shadowRay) const;
	std::optional<Intersection> intersectOctreeNode(uint32_t nodeIndex, const Ray& ray, bool shadowRay, float tEnter, float tExit, float maxT, uint32_t mailboxStamp) const;
	Vec3 traceLocalRay(const Ray& ray, float IOR, int recDepth, bool useOctree) const;

public:
	Scene()
		: Scene(Vec3{0.2f, 0.2f, 0.2f})
	{}

	Scene(const Vec3& backgroundColor)
		: backgroundColor(backgroundColor)
	{ }

	void addObject(std::shared_ptr<const IntersectableObject> object);
	void addLight(std::shared_ptr<const LightSource> ls);
	void buildOctree();
	std::shared_ptr<const LightSource> getLight(size_t index) const;
	void setModel(const Mat4& model);
	Mat4 getModel() const;
	Vec3 getBackgroundcolor() const;
	std::vector<float> getTriangleData() const;
	std::vector<float> getOctreeLineData(uint32_t maxDepth) const;
	std::optional<Intersection> intersect(const Ray& ray, bool shadowRay, bool useOctree) const;
	Vec3 traceRay(const Ray& ray, float IOR, int recDepth, bool useOctree) const;

	static Scene genSimpleScene();

};
