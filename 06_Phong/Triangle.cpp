#include "Triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                   const Shader& s) : v0(v0), v1(v1), v2(v2), shader(s) {
}

void Triangle::draw(Image& image) {
	const float det = (v1.position.y - v2.position.y) * (v0.position.x - v2.position.x) +
                    (v2.position.x - v1.position.x) * (v0.position.y - v2.position.y);

	for (uint32_t y = 0; y < image.height; ++y) {
		for (uint32_t x = 0; x < image.width; ++x) {
			const float a0 = ((v1.position.y - v2.position.y) * (x - v2.position.x) +
                        (v2.position.x - v1.position.x) * (y - v2.position.y)) / det;
			const float a1 = ((v2.position.y - v0.position.y) * (x - v2.position.x) +
                        (v0.position.x - v2.position.x) * (y - v2.position.y)) / det;
			const float a2 = 1 - a0 - a1;

      if (0 <= a0 && 0 <= a1 && 0 <= a2 &&
          1 >= a0 && 1 >= a1 && 1 >= a2) {
        Vertex v;
        v.position = interpolate(v0.position, a0, v1.position, a1, v2.position, a2);
        v.normal = interpolate(v0.normal, a0, v1.normal, a1, v2.normal, a2);
        v.material.color_ambient = interpolate(v0.material.color_ambient, a0,
                    v1.material.color_ambient, a1, v2.material.color_ambient, a2);
        v.material.color_diffuse = interpolate(v0.material.color_diffuse, a0,
                    v1.material.color_diffuse, a1, v2.material.color_diffuse, a2);
        v.material.color_specular = interpolate(v0.material.color_specular, a0,
                    v1.material.color_specular, a1, v2.material.color_specular, a2);

        const Vec3 color = Vec3{ shader.shade(v)};

        image.setNormalizedValue(x, y, 0, color.r);
        image.setNormalizedValue(x, y, 1, color.g);
        image.setNormalizedValue(x, y, 2, color.b);
        image.setValue(x, y, 3, 255);
      }			
		}
	}
}

Vec3 Triangle::interpolate(const Vec3& val0, float a0,
                           const Vec3& val1, float a1, const Vec3& val2,
                           float a2) {
	return val0 * a0 + val1 * a1 + val2 * a2;
}

