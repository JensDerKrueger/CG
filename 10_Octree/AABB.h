#pragma once

#include "Ray.h"

#include <Vec3.h>
#include <algorithm>
#include <cmath>
#include <limits>

class AABB {
private:
    static constexpr float BOUNDS_EPSILON = 0.0001f;
    Vec3 minCorner;
    Vec3 maxCorner;

public:
    AABB()
        : minCorner{std::numeric_limits<float>::max()},
          maxCorner{-std::numeric_limits<float>::max()}
    {}

    AABB(const Vec3& minCorner, const Vec3& maxCorner)
        : minCorner(minCorner),
          maxCorner(maxCorner)
    {}

    const Vec3& getMin() const { return minCorner; }
    const Vec3& getMax() const { return maxCorner; }

    bool isEmpty() const {
        return minCorner.x > maxCorner.x || minCorner.y > maxCorner.y || minCorner.z > maxCorner.z;
    }

    Vec3 center() const {
        return (minCorner + maxCorner) * 0.5f;
    }

    Vec3 extent() const {
        return maxCorner - minCorner;
    }

    bool intersectInterval(const Ray& ray, float& tEnter, float& tExit, float tMin, float tMax) const {
        if (isEmpty())
            return false;

        const Vec3 origin = ray.getOrigin();
        const Vec3 direction = ray.getDirection();

        for (int axis = 0; axis < 3; ++axis) {
            if (std::abs(direction.e[axis]) < 0.000001f) {
                if (origin.e[axis] < minCorner.e[axis] || origin.e[axis] > maxCorner.e[axis])
                    return false;
                continue;
            }

            const float invDirection = 1.0f / direction.e[axis];
            float t0 = (minCorner.e[axis] - origin.e[axis]) * invDirection;
            float t1 = (maxCorner.e[axis] - origin.e[axis]) * invDirection;
            if (invDirection < 0.0f)
                std::swap(t0, t1);

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMax < tMin)
                return false;
        }

        tEnter = tMin;
        tExit = tMax;
        return true;
    }

    bool intersect(const Ray& ray, float tMin, float tMax) const {
        float tEnter = tMin;
        float tExit = tMax;
        return intersectInterval(ray, tEnter, tExit, tMin, tMax);
    }

    bool contains(const AABB& other) const {
        if (isEmpty() || other.isEmpty())
            return false;

        return other.minCorner.x >= minCorner.x && other.maxCorner.x <= maxCorner.x &&
               other.minCorner.y >= minCorner.y && other.maxCorner.y <= maxCorner.y &&
               other.minCorner.z >= minCorner.z && other.maxCorner.z <= maxCorner.z;
    }

    bool overlaps(const AABB& other) const {
        if (isEmpty() || other.isEmpty())
            return false;

        // TODO: Return true if this box and other overlap on all three axes.
        // This is used during octree construction to insert a primitive into
        // every child cell touched by its bounding box.
        (void)other;
        return false;
    }

    void expand(const Vec3& point) {
        minCorner = Vec3::minV(minCorner, point);
        maxCorner = Vec3::maxV(maxCorner, point);
    }

    void join(const AABB& other) {
        if (other.isEmpty())
            return;

        expand(other.minCorner);
        expand(other.maxCorner);
    }

    static AABB fromPoint(const Vec3& point) {
        return AABB{point, point};
    }

    static AABB fromPoints(const Vec3& a, const Vec3& b, const Vec3& c) {
        AABB bounds = fromPoint(a);
        bounds.expand(b);
        bounds.expand(c);
        bounds.minCorner = bounds.minCorner - BOUNDS_EPSILON;
        bounds.maxCorner = bounds.maxCorner + BOUNDS_EPSILON;
        return bounds;
    }

    static AABB join(const AABB& a, const AABB& b) {
        AABB result = a;
        result.join(b);
        return result;
    }
};
