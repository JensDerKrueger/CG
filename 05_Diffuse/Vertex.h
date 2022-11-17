#pragma once
#include "Material.h"

class Vertex {
public:
  // TODO: add the appropriate memeber variables here

  Vertex(const Vec3& position, const Material& material, const Vec3& normal)
  // TODO: initialize member variables here
  {}

  Vertex(const Vec3& position, const Material& material)
  : Vertex(position, material, {0, 0, 1})
  {}

  Vertex(const Vec3& position)
  : Vertex(position, {{1, 1, 1}})
  {}

  Vertex()
  : Vertex({0, 0, 0})
  {}

};
