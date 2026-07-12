#include "Scene.h"
#include "Material.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Triangle.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

Vec3 transformDirection(const Mat4& matrix, const Vec3& direction)
{
  return (matrix * Vec4{direction, 0.0f}).xyz;
}

Material makeDiffuseMaterial(const Vec3& color) {
  return Material{color * 0.25f, color, Vec3{0.18f, 0.18f, 0.18f}, 16.0f};
}

void addTriangle(Scene& scene, const Vec3& a, const Vec3& b, const Vec3& c, const Material& material) {
  scene.addObject(std::make_shared<Triangle>(a, b, c, material));
}

void addQuad(Scene& scene, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Material& material) {
  addTriangle(scene, a, b, c, material);
  addTriangle(scene, a, c, d, material);
}

void addObjects(Scene& scene, const std::vector<std::shared_ptr<const IntersectableObject>>& objects) {
  for (const std::shared_ptr<const IntersectableObject>& object : objects) {
    scene.addObject(object);
  }
}

void appendLineVertex(std::vector<float>& data, const Vec3& position, const Vec3& color, float alpha) {
  data.push_back(position.x);
  data.push_back(position.y);
  data.push_back(position.z);
  data.push_back(color.r);
  data.push_back(color.g);
  data.push_back(color.b);
  data.push_back(alpha);
}

void appendLine(std::vector<float>& data, const Vec3& a, const Vec3& b, const Vec3& color, float alpha) {
  appendLineVertex(data, a, color, alpha);
  appendLineVertex(data, b, color, alpha);
}

void appendBoxLines(std::vector<float>& data, const AABB& bounds, const Vec3& color, float alpha) {
  const Vec3 minCorner = bounds.getMin();
  const Vec3 maxCorner = bounds.getMax();
  const Vec3 p000{minCorner.x, minCorner.y, minCorner.z};
  const Vec3 p100{maxCorner.x, minCorner.y, minCorner.z};
  const Vec3 p010{minCorner.x, maxCorner.y, minCorner.z};
  const Vec3 p110{maxCorner.x, maxCorner.y, minCorner.z};
  const Vec3 p001{minCorner.x, minCorner.y, maxCorner.z};
  const Vec3 p101{maxCorner.x, minCorner.y, maxCorner.z};
  const Vec3 p011{minCorner.x, maxCorner.y, maxCorner.z};
  const Vec3 p111{maxCorner.x, maxCorner.y, maxCorner.z};

  appendLine(data, p000, p100, color, alpha);
  appendLine(data, p100, p110, color, alpha);
  appendLine(data, p110, p010, color, alpha);
  appendLine(data, p010, p000, color, alpha);

  appendLine(data, p001, p101, color, alpha);
  appendLine(data, p101, p111, color, alpha);
  appendLine(data, p111, p011, color, alpha);
  appendLine(data, p011, p001, color, alpha);

  appendLine(data, p000, p001, color, alpha);
  appendLine(data, p100, p101, color, alpha);
  appendLine(data, p110, p111, color, alpha);
  appendLine(data, p010, p011, color, alpha);
}

void addOBJTriangles(Scene& scene, const std::string& filename, const Material& material, const Vec3& scale, const Vec3& translation) {
  std::vector<std::shared_ptr<const IntersectableObject>> triangles = Triangle::loadOBJ(filename, material, scale, translation);
  if (triangles.empty())
    triangles = Triangle::loadOBJ("Datasets/" + filename, material, scale, translation);

  addObjects(scene, triangles);
}

} // namespace

void Scene::addObject(std::shared_ptr<const IntersectableObject> object) {
  sceneObjects.push_back(object);
  bvhObjectIndices.clear();
  bvhNodes.clear();
}

void Scene::addLight(std::shared_ptr<const LightSource> ls) {
  lightSources.push_back(ls);
}

std::shared_ptr<const LightSource> Scene::getLight(size_t index) const {
  if (index >= lightSources.size())
    return {};

  return lightSources[index];
}

void Scene::setModel(const Mat4& model) {
  // This local-space raytracing path assumes translations, rotations, and uniform scales only.
  this->model = model;
}

Mat4 Scene::getModel() const {
  return model;
}

Vec3 Scene::getBackgroundcolor() const {
  return backgroundColor;
}

std::vector<float> Scene::getTriangleData() const {
  std::vector<float> data;

  for (std::shared_ptr<const IntersectableObject> object : sceneObjects) {
    object->appendTriangleData(data);
  }

  return data;
}

std::vector<float> Scene::getBVHLineData(uint32_t maxDepth) const {
  std::vector<float> data;
  if (bvhNodes.empty())
    return data;

  const auto appendNode = [this, maxDepth, &data](const auto& self, uint32_t nodeIndex, uint32_t depth) -> void {
    if (depth > maxDepth)
      return;

    const float t = maxDepth == 0 ? 0.0f : float(depth) / float(maxDepth);
    const Vec3 color{1.0f - 0.35f * t, 0.85f - 0.45f * t, 0.20f + 0.80f * t};
    appendBoxLines(data, bvhNodes[nodeIndex].bounds, color, 0.75f);

    if (bvhNodes[nodeIndex].isLeaf())
      return;

    self(self, bvhNodes[nodeIndex].left, depth + 1);
    self(self, bvhNodes[nodeIndex].right, depth + 1);
  };

  appendNode(appendNode, 0, 0);
  return data;
}

void Scene::buildBVH() {
  bvhObjectIndices.clear();
  bvhNodes.clear();

  // TODO Task 2:
  // Fill bvhObjectIndices with the indices of all scene objects that have a
  // finite bounding box. Do not move the objects themselves; only reorder this
  // index vector while building the hierarchy.
  //
  // If the resulting vector is not empty, call buildBVHNode over the complete
  // range. The returned node index will be the root node and should be 0.
}

uint32_t Scene::buildBVHNode(uint32_t begin, uint32_t end) {
  // TODO Task 2:
  // Build one BVH node for the object-index range [begin, end).
  //
  // Suggested structure:
  // 1. Compute the bounding box of all objects in the range.
  // 2. Compute a second box around the object centroids.
  // 3. Append a BVHNode to bvhNodes and remember its index.
  // 4. If the number of objects is <= BVH_LEAF_SIZE, store firstObject and
  //    objectCount in the node and return its index.
  // 5. Otherwise choose the split axis from the largest centroid-box extent.
  // 6. Use std::nth_element to partition bvhObjectIndices around the median
  //    centroid on the selected axis.
  // 7. Recursively build left and right child ranges and store their node
  //    indices in the current node.
  (void)begin;
  (void)end;
  return 0;
}

std::optional<Intersection> Scene::intersectBruteForce(const Ray& ray, bool shadowRay) const {
  std::optional<Intersection> result{};
  for (std::shared_ptr<const IntersectableObject> object : sceneObjects) {
    if (shadowRay && !object->getMaterial().isShadowCaster())
      continue;

    std::optional<Intersection> i = object->intersect(ray);
    if (!i)
      continue;

    if (!result || i.value().getT() < result.value().getT())
      result = i;
  }
  return result;
}

std::optional<Intersection> Scene::intersectBVHNode(uint32_t nodeIndex, const Ray& ray, bool shadowRay, float maxT) const {
  // TODO Task 3:
  // Traverse the BVH recursively.
  //
  // Suggested structure:
  // 1. Test the ray against the node bounding box. Use maxT as the far clipping
  //    distance, so nodes behind an already known hit can be skipped.
  // 2. If the box is missed, return no intersection.
  // 3. If the node is a leaf, intersect all objects in its stored index range
  //    and return the closest hit. For shadow rays, skip objects whose
  //    material does not cast shadows.
  // 4. If the node is an inner node, recursively traverse both children.
  //    After the first child hit, tighten maxT before testing the second child.
  // 5. Return the closest hit found in either child.
  (void)nodeIndex;
  (void)ray;
  (void)shadowRay;
  (void)maxT;
  return {};
}

std::optional<Intersection> Scene::intersectBVH(const Ray& ray, bool shadowRay) const {
  if (bvhNodes.empty())
    return intersectBruteForce(ray, shadowRay);

  return intersectBVHNode(0, ray, shadowRay, std::numeric_limits<float>::max());
}

std::optional<Intersection> Scene::intersect(const Ray& ray, bool shadowRay, bool useBVH) const {
  return useBVH ? intersectBVH(ray, shadowRay) : intersectBruteForce(ray, shadowRay);
}

/// <summary>
/// Trace a ray through the scene and compute its color value.
/// </summary>
/// <param name="ray">to trace</param>
/// <param name="IOR">optical density of the material we are currently travelling in</param>
/// <param name="recDepth">recursion depth</param>
/// <returns>final color value computed for this ray</returns>
Vec3 Scene::traceRay(const Ray& ray, float IOR, int recDepth, bool useBVH) const {
  const Mat4 inverseModel = Mat4::inverse(model);
  const Vec3 localDirection = Vec3::normalize(transformDirection(inverseModel, ray.getDirection()));
  if (localDirection.sqlength() == 0.0f)
    return backgroundColor;

  const Ray localRay{ inverseModel * ray.getOrigin(), localDirection };
  return traceLocalRay(localRay, IOR, recDepth, useBVH);
}

Vec3 Scene::traceLocalRay(const Ray& ray, float IOR, int recDepth, bool useBVH) const {
  if (recDepth == 0)
    return backgroundColor;

  // no intersection found
  std::optional<Intersection> opt_intersection = intersect(ray, false, useBVH);
  if (!opt_intersection)
    return backgroundColor;

  // else intersection found, do recursive ray tracing
  Intersection inter = opt_intersection.value();
  Vec3 interPos = ray.getPosOnRay(inter.getT());

  Vec3 reflColor{ 0.0f, 0.0f, 0.0f };
  if (inter.getMaterial().reflects()) {
    Vec3 reflDir = Vec3::reflect(ray.getDirection(), inter.getNormal());
    Vec3 reflOrigin = interPos + inter.getNormal() * (Vec3::dot(reflDir, inter.getNormal()) > 0 ? OFFSET_EPSILON : -OFFSET_EPSILON);
    Ray reflRay{reflOrigin, reflDir};
    reflColor = traceLocalRay(reflRay, IOR, recDepth - 1, useBVH);
  }

  Vec3 refractionColor{ 0.0f, 0.0f, 0.0f };
  if (inter.getMaterial().refracts()) {
    const float matIOR = *inter.getMaterial().getIndexOfRefraction();
    std::optional<Vec3> potentialRefDir = Vec3::refract(ray.getDirection(), inter.getNormal(), matIOR);
    if (potentialRefDir) {
      const Vec3 refDir = *potentialRefDir;
      if (Vec3::dot(refDir, inter.getNormal()) > 0) {
        // Ray --> from material into air
        Ray refrRay{ interPos + inter.getNormal() * OFFSET_EPSILON, refDir };
        refractionColor = traceLocalRay(refrRay, 1.0, recDepth - 1, useBVH);
      } else {
        // Ray --> from air into material
        Vec3 inSurfacePos = interPos + inter.getNormal() * -OFFSET_EPSILON;
        Ray refrRay{ inSurfacePos, refDir };
        refractionColor = traceLocalRay(refrRay, matIOR, recDepth - 1, useBVH);
      }
    } else {
      // Total internal reflection
    }
  }


  Vec3 localColor{ 0.0f, 0.0f, 0.0f };
  for (std::shared_ptr<const LightSource> ls : lightSources) {
    const Vec3 offSurfacePos = interPos + inter.getNormal() * OFFSET_EPSILON;
    Ray shadowRay{ offSurfacePos, ls->getDirection(offSurfacePos) };
    std::optional<Intersection> shadowInter = intersect(shadowRay, true, useBVH);

    Vec3 ambient = inter.getMaterial().getAmbient() * ls->getAmbient();

    if (!shadowInter || shadowInter->getT() > ls->getDistance(offSurfacePos)) {
      float d = Vec3::dot(ls->getDirection(offSurfacePos), inter.getNormal());
      Vec3 diffuse = inter.getMaterial().getDiffuse() * ls->getDiffuse() * d;
      diffuse = Vec3::clamp(diffuse, 0.0f, 1.0f);

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

Scene Scene::genSimpleScene() {
  Scene s;

  const auto l = std::make_shared<const PointLight>(Vec3{0.0f, 5.5f, -2.0f}, Vec3{0.22f, 0.22f, 0.22f}, Vec3{1.0f, 1.0f, 1.0f}, Vec3{1.0f, 1.0f, 1.0f});
  s.addLight(l);

  const Material floorMaterial = makeDiffuseMaterial(Vec3{0.55f, 0.55f, 0.50f});
  addQuad(s, Vec3{-12.0f, -1.5f, 3.0f}, Vec3{12.0f, -1.5f, 3.0f}, Vec3{12.0f, -1.5f, -15.0f}, Vec3{-12.0f, -1.5f, -15.0f}, floorMaterial);

  const Material bunnyMaterial = makeDiffuseMaterial(Vec3{0.74f, 0.62f, 0.46f});
  addOBJTriangles(s, "bunny.obj", bunnyMaterial, Vec3{2.65f}, Vec3{-0.25f, -0.17f, -4.2f});

  const Material glassMaterial{Vec3{0.02f, 0.03f, 0.04f}, Vec3{0.62f, 0.78f, 0.95f}, Vec3{1.0f, 1.0f, 1.0f}, 96.0f, 0.04f, 1.52f, false};
  s.addObject(std::make_shared<Sphere>(Vec3{-2.2f, -0.70f, -3.2f}, 0.80f, glassMaterial));
  s.addObject(std::make_shared<Sphere>(Vec3{1.9f, -0.62f, -4.4f}, 0.88f, glassMaterial));
  s.addObject(std::make_shared<Sphere>(Vec3{-1.7f, -0.48f, -6.0f}, 1.02f, glassMaterial));
  s.addObject(std::make_shared<Sphere>(Vec3{1.1f, -0.95f, -6.6f}, 0.55f, glassMaterial));

  const Material mirrorMaterial{Vec3{0.08f, 0.08f, 0.08f}, Vec3{0.2f, 0.2f, 0.22f}, Vec3{0.95f, 0.95f, 0.92f}, 64.0f, 0.25f};
  s.addObject(std::make_shared<Sphere>(Vec3{3.0f, -0.95f, -2.8f}, 0.55f, mirrorMaterial));

  const Material blueMaterial = makeDiffuseMaterial(Vec3{0.18f, 0.38f, 0.82f});
  s.addObject(std::make_shared<Sphere>(Vec3{-3.2f, -1.05f, -5.0f}, 0.45f, blueMaterial));

  const Material redMaterial = makeDiffuseMaterial(Vec3{0.85f, 0.18f, 0.12f});
  s.addObject(std::make_shared<Sphere>(Vec3{2.8f, -1.12f, -5.7f}, 0.38f, redMaterial));

  s.buildBVH();
  return s;
}
