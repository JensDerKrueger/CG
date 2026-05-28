#pragma once
#include <Tessellation.h>

class Triangulate
{
public:
	virtual ~Triangulate() = default;
	virtual Tessellation getMesh() const = 0;
};
