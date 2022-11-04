#pragma once
#include "PhongShader.h"
/**
 * A special shader based on the Phong illumination model that creates bumps in the x-y-plane
 * according to the function:
 *
 * f(x, y) = sin^2(pi * x) * sin^2(pi * y)
 *
 */
class BumpPhongShader : public PhongShader {
public:

	BumpPhongShader(const PhongShader& phong, float cellSize, float bumpHeight);
  virtual ~BumpPhongShader() {}

	// Inherited via Shader
	virtual Vec3 shade(Vertex surface) const override;

};
