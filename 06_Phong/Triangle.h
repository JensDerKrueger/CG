#pragma once

#include "Vertex.h"
#include "Image.h"
#include "Shader.h"

class Triangle {
private:
	Vertex v0, v1, v2;
	const Shader& shader;

  Vec3 interpolate(const Vec3& val0, float a0,
                   const Vec3& val1, float a1,
                   const Vec3& val2, float a2);

public:
  Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
           const Shader& s);
	void draw(Image& image);

};

