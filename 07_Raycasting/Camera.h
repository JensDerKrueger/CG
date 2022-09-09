#pragma once
#include <Vec3.h>

class Camera
{
private:
	Vec3 eyePoint;
	Vec3 viewDir;
	Vec3 upDir;
	float foV;

public:
	Camera()
		: Camera(Vec3(0,0,0))
	{ }

	Camera(const Vec3& eyePoint)
		: Camera(eyePoint, Vec3(0, 0, -1))
	{
	}

	Camera(const Vec3& eyePoint, const Vec3& viewDir)
		: Camera(eyePoint, viewDir, Vec3(0, 1, 0))
	{
	}

	Camera(const Vec3& eyePoint, const Vec3& viewDir, const Vec3& upDir)
		: Camera(eyePoint, viewDir, upDir, 60.0f)
	{
	}

	Camera(const Vec3& eyePoint, const Vec3& viewDir, const Vec3& upDir, float fovDeg)
		: eyePoint(eyePoint), viewDir(Vec3::normalize(viewDir)), upDir(Vec3::normalize(upDir)), foV(fovDeg)
	{
	}

	void setEyePoint(const Vec3& eyePoint);
	void setViewDir(const Vec3& viewDir);
	void setUpDir(const Vec3& upDir);
	void setFoV(float fovDeg);
	void setLookAt(const Vec3 lookAt);
	Vec3 getEyePoint() const;
	Vec3 getViewDir() const;
	Vec3 getUpDir() const;
	float getFoV() const;

};

