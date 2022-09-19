#pragma once
#include <Vec3.h>
#include <Image.h>

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

class Raytracer
{
private:
	int recDepth;
	int numSamplesX;
	int numSamplesY;
	Camera camera;
	Scene scene;

public:
	Raytracer(int recDepth, int numSamples)
		: recDepth(recDepth)
	{
		numSamplesX = (int)sqrtf(float(numSamples));
		numSamplesY = numSamples / numSamplesX;
	}

	void setCamera(const Camera& camera);
	void setScene(const Scene& scene);
	void render(Image& img);

private:
	Vec3 traceRay(const Ray& r);
	Ray computeRay(float x, float y, const RaySetup& rs) const;
	RaySetup computeRaySetup(const Image& img);
};

