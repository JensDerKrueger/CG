#define _USE_MATH_DEFINES
#include "Raytracer.h"
#include <cmath>

void Raytracer::setCamera(const Camera& camera)
{
    this->camera = camera;
}

void Raytracer::setScene(const Scene& scene)
{
    this->scene = scene;
}

void Raytracer::render(Image& img)
{
    RaySetup rs = computeRaySetup(img);

    // TODO: implement the missing parts of this method according to the exercise

}

Vec3 Raytracer::traceRay(const Ray& r)
{
    return scene.traceRay(r, 1.0f, recDepth);
}

Ray Raytracer::computeRay(float x, float y, const RaySetup& rs) const
{
    Vec3 dir{ rs.bottomLeft + rs.dX * x + rs.dY * y };
    dir = Vec3::normalize(dir);
    return Ray{ rs.rayOrigin, dir };
}

RaySetup Raytracer::computeRaySetup(const Image& img)
{
    RaySetup rs;

    Vec3 forwardDir = camera.getViewDir();
    Vec3 upDir = camera.getUpDir();
    float openingAngle = float(camera.getFoV() * M_PI/180.0);
    rs.rayOrigin = camera.getEyePoint();

    float aspectRatio = ((float)img.width) / ((float)img.height);

    Vec3 rightDir = Vec3::cross(forwardDir, upDir);

    Vec3 rowVector = rightDir * (tan(openingAngle / 2.0f) * aspectRatio);
    Vec3 columnVector = upDir * (tan(openingAngle / 2.0f));

    rs.dX = rowVector * 2.0f / (float)img.width;
    rs.dY = columnVector * 2.0f / (float)img.height;

    rs.bottomLeft = forwardDir - columnVector - rowVector;

    return rs;
}
