#pragma once
#include <cstdint>
#include <Image.h>
#include <Vec3.h>

#include "Camera.h"
#include "Scene.h"


struct RaySetup
{
public:
	Vec3 bottomLeft;
	Vec3 rayOrigin;
	Vec3 dX;
	Vec3 dY;
};

enum class ScanDirection
{
	TopToBottom,
	BottomToTop
};

class Raytracer
{
private:
	int recDepth;
	int numSamplesX;
	int numSamplesY;
	Camera camera;
	Scene scene;
	bool useBVH{false};
	ScanDirection scanDirection{ScanDirection::TopToBottom};

public:
	Raytracer(int recDepth, int numSamples)
		: recDepth(recDepth)
	{
		numSamplesX = (int)sqrtf(float(numSamples));
		numSamplesY = numSamples / numSamplesX;
	}

	void setCamera(const Camera& camera);
	void setScene(const Scene& scene);
	void setUseBVH(bool useBVH);
	void setScanDirection(ScanDirection scanDirection);
	void render(Image& img);
	bool renderPixels(Image& img, uint32_t xBegin, uint32_t xEnd, uint64_t& nextPixel, uint32_t maxPixelCount);

private:
	void renderPixel(Image& img, uint32_t x, uint32_t y, const RaySetup& rs);
	Vec3 traceRay(const Ray& r) const;
	Ray computeRay(float x, float y, const RaySetup& rs) const;
	RaySetup computeRaySetup(const Image& img) const;
};

