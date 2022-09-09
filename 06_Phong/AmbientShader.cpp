#include "AmbientShader.h"

Vec3 AmbientShader::shade(Vertex surface) const
{
	return surface.material.color_ambient;
}
