#include "BumpPhongShader.h"


// TODO: Implement the missing methods and the rest of this class as necessary for the assignment.

/**
* @param phong a phong shader to use for the actual shading
* @param cellSize size of the bumps in the x-y-plane measured in pixels
* @param bumpHeight amplitude of the bumps in positive z-direction (out of the image plane)
*/

BumpPhongShader::BumpPhongShader(const PhongShader& phong,
                                 float cellSize,
                                 float bumpHeight)
	: PhongShader(phong)
{
	// TODO
}

// Inherited via Shader
Vec3 BumpPhongShader::shade(Vertex surface) const
{
	// TODO
	return PhongShader::shade(surface);
}
