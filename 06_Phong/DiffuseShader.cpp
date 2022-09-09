#include "DiffuseShader.h"

DiffuseShader::DiffuseShader(const Vec3& light, const Vec3& light_diffuse_color)
    : light(light), ld(light_diffuse_color)
{
}

Vec3 DiffuseShader::shade(Vertex surface) const
{
    Material m = surface.material;
    Vec3 c(0, 0, 0);

    Vec3 N(surface.normal);
    Vec3 L(light - surface.position);
    N = Vec3::normalize(N);
    L = Vec3::normalize(L);

    float d = std::max(0.0f, Vec3::dot(N, L));

    c = c + m.color_diffuse * ld * d;

    return c;
}
