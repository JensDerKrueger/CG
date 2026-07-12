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
        // TODO Task 1:
        // Implement the slab test for an axis-aligned bounding box.
        //
        // For every axis:
        // 1. Intersect the ray with the two planes of the box slab.
        // 2. Sort the two t values so t0 is the near plane and t1 is the far plane.
        // 3. Intersect this interval with the current [tMin, tMax] interval.
        // 4. If the interval becomes empty, return false.
        //
        // Remember to handle rays that are parallel to one slab. A parallel ray
        // can only hit the box if its origin lies between the two slab planes.
        (void)ray;
        (void)tEnter;
        (void)tExit;
        (void)tMin;
        (void)tMax;
        return false;
    }

    bool intersect(const Ray& ray, float tMin, float tMax) const {
        float tEnter = tMin;
        float tExit = tMax;
        return intersectInterval(ray, tEnter, tExit, tMin, tMax);
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
