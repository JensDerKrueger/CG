#define _USE_MATH_DEFINES
#include "Raytracer.h"
#include <algorithm>
#include <cmath>

void Raytracer::setCamera(const Camera& camera)
{
    this->camera = camera;
}

void Raytracer::setScene(const Scene& scene)
{
    this->scene = scene;
}

void Raytracer::setUseBVH(bool useBVH)
{
    this->useBVH = useBVH;
}

void Raytracer::setScanDirection(ScanDirection scanDirection)
{
    this->scanDirection = scanDirection;
}

void Raytracer::render(Image& img)
{
    uint64_t nextPixel = 0;
    renderPixels(img, 0, img.width, nextPixel, img.width * img.height);
}

bool Raytracer::renderPixels(Image& img, uint32_t xBegin, uint32_t xEnd, uint64_t& nextPixel, uint32_t maxPixelCount)
{
    xBegin = std::min(xBegin, img.width);
    xEnd = std::min(xEnd, img.width);
    if (xBegin >= xEnd)
        return true;

    const RaySetup rs = computeRaySetup(img);
    const uint32_t rangeWidth = xEnd - xBegin;
    const uint64_t totalPixels = uint64_t(rangeWidth) * img.height;

    uint32_t renderedPixels = 0;
    while (nextPixel < totalPixels && renderedPixels < maxPixelCount) {
        const uint32_t x = xBegin + uint32_t(nextPixel % rangeWidth);
        const uint32_t row = uint32_t(nextPixel / rangeWidth);
        const uint32_t y = scanDirection == ScanDirection::TopToBottom ? row : img.height - 1 - row;
        renderPixel(img, x, y, rs);
        ++nextPixel;
        ++renderedPixels;
    }

    return nextPixel >= totalPixels;
}

void Raytracer::renderPixel(Image& img, uint32_t x, uint32_t y, const RaySetup& rs)
{
    const int numSamples = numSamplesX * numSamplesY;
    Vec3 color;
    if (numSamples == 1) {
        const Ray r = computeRay(float(x), float(y), rs);
        color = traceRay(r);
    } else {
        for (int sY = 0; sY < numSamplesY; ++sY) {
            for (int sX = 0; sX < numSamplesX; ++sX) {
                const Ray r = computeRay(float(x) + float(sX) / float(numSamplesX),
                                         float(y) + float(sY) / float(numSamplesY),
                                         rs);
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

Vec3 Raytracer::traceRay(const Ray& r) const
{
    return scene.traceRay(r, 1.0, recDepth, useBVH);
}

Ray Raytracer::computeRay(float x, float y, const RaySetup& rs) const
{
    Vec3 dir{ rs.bottomLeft + rs.dX * x + rs.dY * y };
    dir = Vec3::normalize(dir);
    return Ray{ rs.rayOrigin, dir };
}

RaySetup Raytracer::computeRaySetup(const Image& img) const
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
