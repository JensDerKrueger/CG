#include "Camera.h"

void Camera::setEyePoint(const Vec3& eyePoint)
{
    this->eyePoint = eyePoint;
}

void Camera::setViewDir(const Vec3& viewDir)
{
    this->viewDir = viewDir;
}

void Camera::setUpDir(const Vec3& upDir)
{
    this->upDir = upDir;
}

void Camera::setFoV(float fovDeg)
{
    this->foV = fovDeg;
}

void Camera::setLookAt(const Vec3& lookAt)
{
    this->viewDir = Vec3::normalize(lookAt - eyePoint);
}

Vec3 Camera::getEyePoint() const
{
    return eyePoint;
}

Vec3 Camera::getViewDir() const
{
    return viewDir;
}

Vec3 Camera::getUpDir() const
{
    return upDir;
}

float Camera::getFoV() const
{
    return foV;
}
