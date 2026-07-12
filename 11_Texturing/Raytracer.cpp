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

    int numSamples = numSamplesX * numSamplesY;
    for (uint32_t y = 0; y < img.height; ++y)
    {
        for (uint32_t x = 0; x < img.width; ++x)
        {
            Vec3 color;
            if (numSamples == 1)
            {
                Ray r = computeRay(float(x), float(y), rs);
                color = traceRay(r);
            }
            else
            {
                for (int sY = 0; sY < numSamplesY; ++sY)
                {
                    for (int sX = 0; sX < numSamplesX; ++sX)
                    {
                        Ray r = computeRay(x + sX / ((float)numSamplesX), y + sY / ((float)numSamplesY), rs);
                        color = color + traceRay(r);
                    }
                }
                color = color / float(numSamples);
            }
            img.setNormalizedValue(x, y, 0, color.r);
            img.setNormalizedValue(x, y, 1, color.g);
            img.setNormalizedValue(x, y, 2, color.b);
            img.setValue(x, y, 3, 255);
        }
    }
}

Vec3 Raytracer::traceRay(const Ray& r)
{
    return scene.traceRay(r, 1.0, recDepth);
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
